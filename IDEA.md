# Key Features

The information provided in this section utilizes calculations and maps similar to that of the HTG GCU, albeit optimized for use on our custom TCU.

# CAN Data

In order for proper control of the gearbox, we need to be able to take a few parameters from the ECU, as well as provide it with some data. This system will be optimized for use with Haltech ECUs.

From CAN

1. Engine RPM
1. Manifold Absolute Pressure
1. Intake Air Temperature
1. Throttle Position
1. Pedal Position
1. Effective Injection Time
1. Ethanol Percentage
1. WB O2 sensors 1 & 2
1. Engine Limiter Max RPM
1. Brake Switch Status

To CAN

1. Current Gear [IOB AVI 1]
1. Transmission Temperature [IOB AVI 2]
1. Cut Request [IOB AVI 3]
1. Blip Request [IOB AVI 3]
1. AUX Rev Limit [IOB DPI 1]
1. Vehicle Speed Sensor [IOB DPI 2]
1. Input Shaft Speed Sensor [IOB DPI 3]

# Shifting Methods

The 6HP family of transmissions must be shifted in a "clutch to clutch" style of shifting, similar to a DCT. For this, we must "fade" between clutches in order to shift. Since multiple clutches or brakes are used to shift between gears, the timing of these gear shifts are the same across all clutches and are specified by the gear.

## Open Loop

Using an open loop style tuning calculation, we can split the shifting into the following phases:

1. **Prefill**
1. **Shift**
1. **Post-shift**

![timing graph](htg_graph.jpg)
Graph from [HTG's wiki](<https://update.htg-tuning.com/wiki/index.html#t=HTG_GCU_User_Guide%2FHTG_wiki_(internal)%2FHTG%2FShifting_tuning.html>)

### Prefill

In order to quickly perform a shift with minimal slipping and minimal RPM drop, it is important that our clutch packs are as close to engaged as we can possibly get. Once a shift is requested from the ECU, the prefill cycle begins. This leaves us with the following maps:

1. Prefill Pressure [%]&nbsp;&nbsp;&nbsp;&nbsp;Axes: Torque, Gear
1. Prefill Time [ms]&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Axes: Torque, Gear

In our simplified use case, the prefill pressure of the outgoing gear is simply 100% - [prefill pressure of incoming gear]

### Shift

Since our shifting style is similar to that of a DCT, we must "fade" the clutches appropriately. Each gear can be controlled to allow for different engagement and disengagement speeds. We also allow for the offset of gear transition to be changed, which allows for the gear aggressiveness to be altered. This leaves us the following maps:

1. Rise Time [ms]&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Axes: Torque, Gear
1. Fall Time [ms]&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Axes: Torque, Gear
1. Spread Time [ms]&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Axes: Torque, Gear

### Post-shift

To allow for better gear engagement, due to the physical limitations of the transmission, we do not fully engage the incoming clutch until after a specified period of time. We hold the clutch at a "shift pressure" for a given period of time, and then fade it to being fully engaged over time. The post-shift time and shift pressure time are identical. In the event that there is no more slippage in the transmission (input RPM / gear ratio = output RPM +- acceptable error), we stop holding it at shift pressure and use the table to ramp to clutch pressure. To perform this operation, we have the following maps:

1. Shift Pressure [%]&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Axes: Torque, Gear
1. Shift Pressure Time [ms]&nbsp;&nbsp;&nbsp;&nbsp;Axes: Torque, Gear


#### ALL GEAR SHIFTS END AT 100% CLUTCH ENGAGEMENT

## Closed Loop

Closed Loop style tuning is a form of "auto tuning" where the controller "learns" how to shift under different circumstances. All shifting parameters are functions of engine torque and desired shift time. Its shifting stages are the same as open loop, however the parameters are determined over time rather than by the tuner. 

The ability to adjust shifting characteristics is categorized on a gear by gear basis with a synchronization rate table. Based off the load and current gear, the rate of RPM drop on US or rise on DS is specified. 

CL tuning can be broken down into the following discrete steps:

1. **Static Clutch Engagement Learning**
1. **Gear Ratio Learning**
1. **Shift Learning**

### Static Clutch Engagement Learning

To properly perform prefill engagement, we must know at what shift pressure the clutches start to engage. To do this, there is a adaptation process that must occur. It is triggered by software and occurs when the car is in drive and the brake is applied. It should be performed at operating temperature.

Clutches are engaged in a specified order. They are engaged incrementally until the RPM starts to fall beneath a certain threshold. For the 6HP26 M-type, the engagement procedure is as follows:

| Solenoid | Procedure |
| :--------: | ------- |
| Clutch A | Engage Brake D (1st gear) |
| Clutch B | Engage Brake D (R gear) |
| Brake C | Engage Clutch A (2nd gear) |
| Brake D | Engage Clutch A (1st gear) |
| Clutch E | Engage MV1 and Clutch C (6th gear)

### Gear Ratio Learning

Using a base table of gear ratios is great, however due to many factors, those gear ratios will not be 100% accurate. Using the Input Shaft RPM and the Output Shaft RPM as well as the current gear will allow us to take samples when driving at a consistent RPM. When the torque converter is locked up and the RPM is not changing more than a certain delta, at a certain interval, the RPMs will be captured and the gear ratio will be calculated using a moving average.

$$ \text{gear ratio} = \frac{RPM_\text{in}}{RPM_\text{out}} $$

### Shift Learning

Using the calculated gear ratios, we can estimate what the input RPM should be after a shift. If the measured RPM is too high, the shift did not occur fast enough. If the measured RPM dips down from our estimate, we have created too much drag and thus need to adjust shift times. 

In order to complete the shift, we need the rise and fall times, prefill % and prefill time, shift pressure and shift time. Spread is not currently used in CL and may be implemented in some form of sport mode. 

Rise time is a function of the overall amount of time we have to complete the shift, with some time subtracted in order to complete a predefined part post-shift stage. Post shift stage might be interrupted, so we use a scaling factor to estimate how much of it we must complete. 

$$ \text{rise time (ms)}=\frac{RPM_\text{drop}}{RPM_\text{sync}}-(\text{shift time}\cdot \text{scale factor})$$

Fall time is our main adaptable variable when determining how to mitigate RPM drop or flare. For example, if the RPM flares up too much, we have released the off-going clutches too soon. If the RPM drops, we have too much overlap and must release the off-going clutches faster. 

This is where our adaptation function comes in. We store values over time for each gear as a function of load and rise time. Based on the load and rise time, we get a fall time returned. If the fall time caused RPM drop or rise, we can lower or raise the fall time we just used in hopes that it improves shift quality. We must make sure that the value cannot change too quickly and that it stays within a programmer specified range. This is why calculated engine load must be accurate, otherwise we cannot make predictions about shift characteristics. 

Prefill percent is calculated during static adaptation, while prefill time is not tuned by CL tuning and is statically set.

Shift pressure and shift pressure time is not adjusted by adaptations and should be statically set. The controller might not hold the solenoid at shift pressure for the entirety of shift pressure time, although the transition to full engagement will always be a function of the shift pressure time table. 

## Cut and Blip

The controller has the ability to request cuts and blips from the Haltech ECU.

# Torque Converter Clutch Tuning

The TCC frequency and ramp rate must be specified, as they can be constants to avoid instant engagement of TCC and allow for smoother lockup. 

### Engagement Tables

There are two tables that have the current gear and the TPS as the axes, one for engagement and one for disengagement. Any value below the disengagement RPM will disengage the TCC and any value above the engagement RPM will engage it. TPS is used since we care more about where the engine is "going" rather than the current load.

### Special Engagement Features

Whenever the brake is applied or the PPS reads an input above a certain threshold, the TCC can be unlocked. The TCC can also be forced to unlock during shifting. Using the brake or PPS allows for the TCC to be unlocked before the disengagement RPM, allowing for easier transitions on and off power. TCC re-engagement threshold can also be specified, blocking the TCC from locking up after a brake press until either the TPS or PPS reaches a certain threshold.

# Line Pressure Tuning

Line pressure can be modulated in order to affect clamping power and shift smoothness. There are multiple tables that are setup to do this under different conditions. Solenoid frequency and ramp rate must also be specified, as they can be constants.

### In Gear Target

This is a table that takes engine load and the current gear as inputs. It returns line pressure as a percent. Higher numbers indicate higher line pressure.

### Upshift Target

This is a table that takes engine load and the number of the gear we are going to upshift into. It returns a line pressure as a percent. Higher numbers indicate higher line pressure.

### Downshift Target

This is a table that takes engine load and the number of the gear we are going to downshift into. It returns a line pressure as a percent. Higher numbers indicate higher line pressure.

### Temp Correction

This is a multiplier table that adjusts the line pressure globally. As gear oil gets warmer, it becomes lighter and line pressure must increase. Higher values result in higher final pressure.

# Automatic Shift Tuning

This controller allows for fully automatic shifting, as well as paddle shift overrides.

### Shifting Schedule / Shift Variable

Our main shifting schedule is influenced by our main shifting variable. In this case, our shifting variable should be vehicle speed. When provided with a final gear ratio and a tire total diameter (which can be calculated [here](https://tiresize.com/calculator/)) the vehicle speed can be calculated.

Two shifting schedules are provided, one for upshifts and one for downshifts. They are both functions of engine load and gear. They return a speed at which the shift should occur. Downshifts occur anywhere below the given speed and upshifts occur at any value above the given speed. Higher engine loads require higher vehicle speeds before upshift to stay on the power band for longer. The same is true for downshifts, as a higher RPM delta is more acceptable when under a larger load.

### Shifting Enrichments

Shifting enrichments include using PPS as an override for the shift schedule. When the PPS reaches a certain threshold, the controller will try and perform a downshift to the lowest possible gear. This gear is determined using a maximum possible input shaft speed. The gear with the closest value to this RPM, while still being under it, is determined to be the ideal gear. Theoretically, this should only be engaged at WOT as the shifting schedule should be tuned for most operations. 

### Paddle Shift

Paddle shift is an override for the current shift schedule. Paddle shift limits include a maximum RPM that is separate from the over-rev limit, as well as an auto-reset time, at which if an input has not been pressed, the controller will return to the normal shift schedule. Paddle shift disables all shifting enrichments. 

### Gear Skipping

Gear skipping is still a WiP. Physically, it seems as if the transmission is capable of it. Some manufacturers seem to have it while others do not. Currently, no gears are allowed to be skipped. Using the 8HP as an example, all valid multi gear shifts only allow for the transition of a single shift element at a time. All gears can up or downshift 1 or 2 gears at a time, but only certain gears can go further than that. For example, 8<->6, 8<->4, and 8<->2 are valid, but 8<->3 and 8<->5 are not. 

Applying this logic to the power flow of the 6HP, we receive the following valid substandard shifts: 1<->3, 2<->4, 3<->5, 4<->6, 2<->6. Allowing these gear shift very much complicates the control the the MV1 solenoid. These gear shifts have also not been verified against the power flow of the 6HP either. 

### Lockout

Lockout defines a constant time the algorithm must wait after a shift to perform the next shift in the schedule. It can be override by the RPM protection however.

# Safety Methods

### Limp Mode

The conditions for limp mode are currently under development, however the desired protections for limp mode are as follows:

1. TCU imposed engine auxillary rev limit
1. Stuck to either 4th or 5th gear (shift schedule bypassed)
1. Shift between 4th and 5th gear is slower (all CL tables disabled)
1. No TCC lockup
1. No gear skipping (stuck in 4th/5th anyways)
1. No paddle shifting
1. Possible limitation of line pressure modulation

### Over-Rev

There are two protections for over-rev. One that sets a maximum RPM where an upshift is required regardless of shift table, and another that sets the RPM at which a singular downshift can occur (this is an offset of max RPM). For example, if our max shift RPM is 7500RPM and our downshift threshold is 1000RPM, this means for a downshift to be viable, the estimated engine RPM after the shift needs to be lower than 6500RPM. Both Engine RPM and Input Shaft Speed are used for this calculation. Whichever value is higher is the one used (meant as a failsafe for the CAN bus).

### Mode Transition

This is a map of conditions that must be met before the controller shifts into this next gear set. Both axes are Park, Reverse, Neutral, and Drive. Since on the M-type 6HP transmissions, the only physical engagement controlled by the shift lever is when the transmission is placed in park, we can specify all of the restrictions between gear changes regardless of the shift lever input. 

Transition requirements at this time only include vehicle speed requirements and brake pedal. This means the transmission will not go to the gear until the speed is below the requirement or the brake is being pressed. Since the park pawl is physical, this really only helps to prevent putting the car in reverse or neutral unless the car is moving slowly enough. Also since there is no requirement for shifter locking, there is no feedback to the user about the fact the brakes need to be pressed for certain gear changes, so use sparingly. 

### Solenoid Check Function

This is a constantly running check function that mandates that all solenoid outputs, regardless of what the "calculated value" is, fits within a specified table of acceptable outputs. This is to prevent internal damage to the transmission.

# Torque Estimation Algorithm

In order to accurately determine shifting characteristics, we must modulate the shifting speed based on estimated engine torque. 

## Fuel Based Model

For the fuel based model, our estimation of engine torque is realistically just a function of engine demand. We use multiple sources of engine demand to accurately calculate the amount of load on the engine. These sources include:

1. Utilized Air Mass
1. Volumetric Efficiency
1. Theoretical Fuel Performance
1. ECU Reported Engine Demand

Maximum and minimum values for certain load metrics are stored in order to normalize all values between 0% and 100%. The values are then averaged together using predetermined weights. VE and ECU Engine Demand are not normalized. Theoretical Fuel Performance max is simply max flow rate * max fuel efficiency. Utilized Air Mass is the only metric where we must store max and min values. 

The required inputs to the function are:

1. Engine Displacement [cc]
1. Cylinder Count
1. Engine Limiter Max RPM
1. Injector Flow Rate [cc/min]
1. Flex Fuel Sensor [Ethanol %] (Can be disabled)
1. Fuel Density Map [mg/cc] (Ethanol % Lookup)
1. Fuel Efficiency Map [MJ/kg] (Ethanol % Lookup)
1. Engine RPM
1. MAP [kPa ABS]
1. Actual Lambda [λ] (WBO2 1/2 average)
1. Effective Injection Time [μs]
1. ECU Reported Engine Demand [%]

The torque calculation can be broken down into the following steps:

1. Effective Fuel Mass
1. Theoretical Air Mass
1. Reported Air Mass
1. Volumetric Efficiency
1. Maximum Energy Released
1. Calculated Load

### Effective Fuel Mass

To calculate the total maximum fuel flow rate, we first convert the fuel flow rate from cc/min to cc/s. We then multiply this by the number of cylinders to calculate maximum theoretical injector flow rate in volume.

$$ \text{max fuel flow rate (mg/s)}=\frac{IFR \cdot N}{60} \cdot FD $$

Where IFR is injector flow rate in cc/min, N is number of cylinders, and FD is fuel density in mg/cc.

We then need to calculate the total time of an engine cycle in μs:

$$ \text{cycle time (μs)}=\frac{60 \cdot 2 \cdot 1000000}{RPM}$$

We then use the effective injector time reported by the ECU in μs to calculate a dimensionless duty cycle:

$$ \text{duty cycle}=\frac{IT}{\text{cycle time}} $$

The mass flow rate of the fuel can be calculated using the duty cycle and the maximum injection flow rate:

$$ \text{fuel flow rate (mg/s)}=\text{max fuel flow rate} \cdot \text{duty cycle} $$

### Theoretical Air Mass

Theoretical air mass is not indicative of engine load, however we can use it to calculate VE. Using the Ideal Gas Law:

$$ PV = nRT $$

P is pressure in Pa (MAP in kPa), V is volume in cm, n is moles of gas, R is ideal gas constant, and T is temperature in K. We can rearrange it to solve for the number of moles of air at a given pressure, temperature, and volume. We can use the molar mass of air (28.96 g/mol) to convert this to grams.

$$ n=\frac{28.96 \cdot PV}{RT}\text{, where n is mass of air in grams} $$

Since we want air mass FLOW, we want this formula to give us a value with a unit of grams/second. We can do this simply by replacing the volume (V) with a volume with respect to time.

$$ V = \frac{RPM}{60 \cdot 2} \cdot D \cdot 0.000001 $$

Given displacement (D) in cc, this will give us cm of air/sec. RPM is divided by 2 since in a 4-stroke engine, every 2 rotations is an entire cycle.

When combining these formulas, we can calculate theoretical mass air flow in g/s using the following formula:

$$ MAF_\text{thr}(g/s)=\frac{28.96 (MAP \cdot 1000) \cdot RPM \cdot D \cdot 0.000001 }{8.314462 \cdot IAT \cdot 60 \cdot 2} $$

### Reported Air Mass

We can use the air/fuel ratio and the fuel mass to get an accurate estimation of the actual air mass used. This also allows us to calculate VE.

We first take the average of both WBO2 sensors, producing a lambda value (value relative to 14.7). We then use our calculated effective fuel mass to calculate our actual used air mass.

$$ MAF_\text{reported}(g/s)=\lambda \cdot 14.7 \cdot \text{fuel flow rate (mg/s)} \cdot 0.001 $$

### Volumetric Efficiency

In properly tuned engines, VE is almost identical to the torque curve. We used the reported air mass along with the theoretical air mass to calculate VE.

$$ VE=\frac{MAF_\text{reported}}{MAF_\text{calculated}}$$

### Maximum Energy Released

Depending on the fuel being used, the amount of energy for a given air mass might not produce the same amount of power. For example, E85 will use more fuel to net the same amount of power. We use the ethanol percentage to use a lookup table to find the energy density of our current fuel blend. While it is true, E85 can make more power under certain circumstances due to spark timing/higher octane, for our NA application with VE < 100%, fuel usage, when averaged by air usage, is a "good enough" estimate of total energy produced. Also, since engines are not even close to 100% efficient, this value is not close to accurate, but since it's normalized, it can be used to create a relative load estimation. Realistically, the efficiency of the engine is not constant and the effectiveness of the fuel decreases with lower VE, so just because we are injecting more fuel does not mean we are making more power. We are making an assumption by using fuel flow rate as one of our four load calculations, and we are simply using total maximum energy as a method to scale based on our fuel type.

$$ E_m\text{(MJ/s)}=\text{fuel flow rate (mg/s)} \cdot \text{fuel efficiency (MJ/kg)} \cdot 0.000001 $$

### Calculated Load

We first start by normalizing all values between 0 and 1. Only Em and MAF need to be normalized. If our current RPM is within range of the provided rev limiter, we use our last calculated engine load to prevent a situation where fuel is cut and engine load numbers are inaccurate.

$$ \text{normalized value}=\frac{x-min}{max-min} $$

Currently, weights are evenly distributed. This means the engine load can be determined with the following algorithm:

$$ \text{engine load (percent)}=0.25MAF_\text{reported} \cdot 0.25VE \cdot 0.25E_m \cdot 0.25\text{ECU}_d$$
