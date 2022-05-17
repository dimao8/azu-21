# azu-21

AZU-21 is an automatic charger for Li-ion accumulator battaries with configuration 4s1p and 5s1p.

The charger is automatically check the connected battery and decide to charge it or light an _error_ LED
if battery is bad. When charging in process there is three stages: precharge, current charge and voltage charge.

__Precharge__ stage: charger check out battery state ang then increase charging current from 0A to maximum
charging current (default 0.5A). The stage duration is about 1min.

__Current charge__ stage: charger continue to pass maximum current through the battery. Good battery will increase
its voltage to maximum (typically 4.2V for one Li-ion battery).

__Voltage charge__ stage: charger try to hold constant voltage on battery. During this process the current will be decrease
until riches its minimum (about 50mA). Then charger stop charging process.

In idle state charger do not charge battery. 7-segment leds show maximum charging current and fully charged battery
voltage. User can change this values by pressing _MODE_ button and then press _UP_ or _DOWN_ buttons. The changing
value will blink with 1s period. When done pressing _MODE_ will return charger to idle state. Red display shows current
value. Green display shows voltage.

Start charging is performed by pressing _START_ button. When charging in progress the _charging_ LED will turn on.
If there is some trouble during charging, the _error_ LED will turn on, and charging process will be terminated.

During charging process 7-segment displays show current and voltage on battery during charging.

The charger measure battery temperature with thermal sensors in the battery case. If temperature reaches about 70 celsius
the charging is termanated with error.

NOTE: The battery must have battery ballance device inside its package.
