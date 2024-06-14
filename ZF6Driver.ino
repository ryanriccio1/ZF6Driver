#include "HTCANSource.h"
#include "Constants.h"


IDataSource *data_source;
void setup()
{
    data_source = new HTCANSource();
    // set start time
}

void loop()
{
    // on gear shift initiated, set var
    if (true)
    {
        // currently_shifting = true

        // get data from data source
        data_source->get_engine_demand();

        // get map information
        // var1 = map1(data);
        // var2 = map2(data);

        // calculate rise increment(prefill percentage, rise time, shift pressure)
        // calculate fall decrement(prefill percentage, fall time)
        // calculate post shift time rise(shift pressure %, post shift time)

        // set stage to prefill
    }
    if (true) // currently_shifting
    {
        // if prefill stage
        // {
        //      if prefill stage starting
        //      {
        //          set prefill start time
        //          raise/lower pressure according to map
        //      }
        //      else if current time >= prefill start time + map time
        //      {
        //          prefill done, shift stage start
        //      }
        // }

        // if shift stage
        // {
        //      if shift stage starting
        //      {
        //          set shift stage start time
        //          last run time = current time
        //          if spread != 0
        //          {
        //              if spread > 0
        //              {
        //                  rise start time = shift start time + spread
        //                  fall start time = shift start time
        //              }
        //              else if spread < 0
        //              {
        //                  rise start time = shift start time
        //                  fall start time = shift start time + spread
        //              }
        //          }
        //      }
        //      // has a ms elapsed or are we on our starting run
        //      if last run time + 1 >= current time || stage start time >= current time
        //      {
        //          if current time >= rise start time
        //          {
        //              increment incoming clutches
        //          }
        //          if current time >= fall start time
        //          {
        //              decrement outgoing clutches
        //          }
        //          last run time = current time
        //      }
        //      if current time == rise start time + rise time && current time == fall start time + fall time
        //      {
        //          // make sure our shift pressures are 100% correct
        //          set incoming clutches to shift pressure
        //          set outgoing clutches to 0
        //          shift stage done, post shift start
        //      }
        // }

        // if post shift stage
        //{
        //      if post shift stage starting
        //      {
        //          set post shift start time
        //          set post shift increase start time = post shift start time + shift pressure time
        //      }
        //      if current time >= post shift increase start time && last run time + 1 >= current time
        //      {
        //          increment incoming clutch
        //          last run time = current time
        //      }
        //      if post shift increase start time + post shift time >= current time
        //      {
        //          set incoming clutches to 100%
        //          post shift stage done, shift procedure done, currently shifting done
        //      }
        //}

        // else currently shifting = done
    }
}
