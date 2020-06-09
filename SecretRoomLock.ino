/*
* This code is for the project at 
* http://www.iliketomakestuff.com/how-to-make-a-secret-room-through-a-wardrobe
* All of the components are list on the url above.
* 
* We offer NO SUPPORT for this code, and do not promise any updates/improvements.
* Please regard this as "unoptimized, prototype code".
* 
*This script was created by Bob Clagett for I Like To Make Stuff
*For more projects, check out iliketomakestuff.com
*/
// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2
/* The Bounce2 library looks to be intended to ease programming against
   Arduino.  Think of it like an API.  This line includes it.  In C, you
   include the "header" files, but not the 3rd party code itself, directly.
   It's a little weird if you haven't done it before.  Header files
   are kind of like a concise description of the functions provided by a
   chunk of code.  For now, just roll with it.
*/
#include <Bounce2.h>

/* These define lines are declaring constants.  When the code is compiled,
   every reference to "BUTTON_PIN" is substituted for the number 14.  It's
   a shorthand for actually typing "14" everywhere.  "BUTTON_PIN" does not
   actually exist as a variable - you can't change its value in the code.
   This is why they're in all-caps.  That's convention in C to indicate it's
   a constant, and can never be changed.  It's a lot easier to read "LED_PIN"
   in the code though, than unexpectedly encountering the magic number "12"
   without explanation.  You *could* add a comment explaining why you used
   "12" in-line, or you could just declare a constant.
   TL;DR: These constants are swapped out for the numbers they represent,
   at compile-time.  The variable "BUTTON_PIN" never exists, because by
   run-time, we don't see "BUTTON_PIN" - we just see the number 14.
*/
#define BUTTON_PIN 14
#define BUTTON_PIN2 16
#define LED_PIN 12
#define RELAY_PIN 13

/* C is a statically-typed language, which means when you declare a
   variable, you also have to declare the data-type that variable will
   be used to contain.  For example, "lockEngage" is declared as a
   boolean here, so it can only ever hold one of two values.  In C,
   0 means False, and everything else means True.  A boolean is
   implemented numerically by the compiler.  There doesn't actually
   exist a "value" of True or False in C.  As such, you *could*
   actually do something like, "if (1+1) { ...etc"
   Anyway, an "int" is a signed number, and is allocated the same
   amount of storage as the CPU architecture (ie: 4-bytes on a 32-bit
   machine.  8-bytes on a 64-bit machine).  This variability between
   the size of an "int" on a 32- vs 64-bit machine is one of the
   reasons that software comes in both 64-bit and 32-bit versions.
   Note that the use of "LOW" and "HIGH" as "int" values is not
   valid C or C++.  That's something specific to Arduino:
   https://forum.arduino.cc/index.php?topic=111711.0
   In that forum post, they mention that HIGH == 1 and LOW == 0.
   That's actually because HIGH/LOW are being treated as booleans.
   HIGH == non-zero == True
   LOW == 0 == False
   Because of that, we can do things like "!(HIGH)", meaning
   "not-HIGH", aka "not-True", aka "False".  And vice-versa.
   The use of "HIGH" and "LOW" is an Arduino-ism, that reflects
   the electrical logic - if the voltage is high (~3 to 5 volts),
   that's considered a "1".  If the voltage is low (<1 volt), we
   consider that a 0.  That's a universal assumption in all
   computing.  In code, we treat 0 and 1 as hard & fast numbers,
   but really we're talking about the electrical charge on a transistor,
   and that's not nearly as discrete.  So in short, HIGH & LOW
   are really just the Arduino's electrical-engineering concepts of
   True & False.

   All the boolean terminology can get confusing here.
   Remember:
   HIGH == True  == non-Zero == Powered (locked magnet, lit LED, etc)
   LOW  == False == Zero     == Unpowered (unlocked, un-lit, etc)

   That's just for this code - depending how you write *your* code,
   HIGH might mean Unlocked.
*/
boolean lockEngage = false;
int lockState = LOW;
int debounceInterval = 25;

/* This bit introduces object-oriented coding techniques, for which
   entire books have been written.  Honestly, you should either try
   to figure it out as you go, or dedicate some time to learning/
   practicing Object-Orientied programming (OO).  Good languages
   for OO are Python, Ruby, C++, or Java.
   If you want to learn it properly, your best bet is to google
   up some professor's CS1 homework assignments in an Object-
   Oriented programming course.  Most of that content is publicly
   accessible, on the college's website. (something.edu)
*/
Bounce debouncer = Bounce(); // Instantiate a Bounce object
Bounce debouncer2 = Bounce(); // Instantiate a Bounce object


/* This is the first function definition.  In C, you define a
   function by indicating what data-type it returns, then the 
   name of the function, then any arguments that function requires
   inside the parens.
   ie: int half(int x) { return x / 2; }
   The keyword "void" means this function doesn't return anything.
*/
void setup() {

  // Serial must be another Arduino-specific mechanism.
  // It's not a C built-in, and we didn't Instantiate or #include it.
  Serial.begin(9600);  

  /* debouncer & debouncer2 are both objects - instances of the Bounce class
     We instantiated these above.  You'll have to consult the Bounce
     code and/or documentation for information on exactly how to interact
     with these instances of the Class.
  */
  debouncer.attach(BUTTON_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  debouncer2.attach(BUTTON_PIN2,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  debouncer.interval(debounceInterval);
  debouncer2.interval(debounceInterval); 

  /* This is initializing everything to the correct starting-state.
     The Arduino has a bunch of GPIO pins, and each pin can be
     configured as either a signaling pin, or a sensor (and probably
     others).  In this case, we're assigning "BUTTON_PIN" (ie: Pin #14)
     and BUTTON_PIN2 (ie: Pin #16) as sensors.  When we hit one of those
     buttons, it'll send a low-voltage signal to the GPIO pin, and since
     we've put those pins in sensor-mode, that'll cause the Arduino to
     notice the button was pushed.
     Q: Why #14 and #16?
     A: All pins on the Arduino are numbered, and we've got 2 buttons,
        so we had to pick 2 pins.  14 & 16 were the ones Bob/Josh felt
        comfortable soldering onto.  It didn't have to be these two
        pins, but you *do* have to ensure the physical connections
        match up to the pin #s you're using in the code.  This is another
        reason constants are useful - if we decided to use pin #15
        instead, we wouldn't have to search our code for every mention
        of the number "15" - we can just change the constant at the top.
  */
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  pinMode(BUTTON_PIN2,INPUT_PULLUP);

  /* Same here - initialize the pins.  These two we're using in OUTPUT
     mode, which means we won't be reading info *from* these pins.  The
     Arduino will send a signal *out* these pins - the the LED & Relay,
     accordingly.
     Think of the pinMode() function as telling the Arduino to use that
     pin, and whether it's going to send or receive on that pin.
  */
  pinMode(RELAY_PIN,OUTPUT); // Setup the LED
  pinMode(LED_PIN,OUTPUT); // Setup the LED

  /* Way up at the top, we initialized "lockState=LOW", so in these 2
     lines we're initializing both the RELAY & LED to "LOW" or "False"
     mode.  ie: Door unlocked and LED off.
  */
  digitalWrite(RELAY_PIN,lockState);
  digitalWrite(LED_PIN,lockState);

  /* This line was probably just used for testing, when they probably
     had a monitor attached, pre-door.  It's not important."
  */
  Serial.println("cool beans");
}

void loop() {
  /* See the Bounce2 code for specifics, but essentially these
     update() lines are fetching info on what's happened with the
     buttons since we last checked (which was probably like, 1/100th
     of a second ago.  This loop() function will run repeatedly,
     very rapidly.
     If we forgot to run update() on both the Bounce2 objects,
     then our code would never notice when the buttons were pushed.
  */
  debouncer2.update();
  debouncer.update(); 

  /* This appears to be dead-code.  Bob/Josh probably used it for testing,
     and forgot to delete it.  Ignore.
  */
  // Get the updated value :
  int value1 = debouncer.read();
  int value2 = debouncer2.read();

  /* debouncer & debouncer2 are essentially listening to the buttons.
     Running ".fell()" is basically asking each button - 
     "Hey, were you pushed since I last checked with you?"
     If either button returns True (ie: either button was pushed),
     this evaluates to True, and we run the stuff inside the if().
     Note - it may seem weird that we only ever test if the button
     goes from "HIGH" to "LOW" - we never check for the button's
     signal going back to HIGH.  That's because this is a momentary
     switch, so the signal from the button is sitting in HIGH at
     rest - when you press and hold the button, it switches signal
     to LOW, and *keeps* sending LOW until you let go, then it 
     switches back to HIGH.  In some cases, a person letting go
     of the button *could* be useful information, but because *this*
     use-case is just to *toggle* the door between locked/unlocked
     every time the button gets pressed(and likely immediately released),
     we don't actually care about LOW to HIGH.
     TL;DR: ".fell()" means "button-was-just-pushed"
  */
  if ( debouncer.fell() || debouncer2.fell()) {  // Call code if button transitions from HIGH to LOW
    // Debug output - doesn't do anything useful.
    Serial.println("click");

    /* As mentioned higher up, we can negate lockState with "!", so if
       it was True, it becomes False, and vice-versa.
       Keep in mind, each button is just a momentary switch.  The
       buttons don't keep track of whether the door is locked or
       not.  We sense when buttons get pushed, but we control in
       this code, whether the door is locked or not, by keeping
       track of lockState.
       We could also do something like:
       if(locked){unlock}
       if(unlocked){lock}
       But by treating lockState as a boolean, we can use ! to negate,
       which lets us say "Set to the other one".  Less code ftw.
    */
    lockState = !lockState; // Toggle lock state

    /* Note here, that "==" is comparing.  All use of "=" above is
       saying "set equal to", but this one is "check if equal to".
       Get used to that in C.
    */
    if (lockState == HIGH){
      /* Since we just negated lockState, this conditional is checking
         if we *just* changed from LOW to HIGH - meaning, did we just
         lock the door?  If so, we introduce a delay.  This is how the
         door unlocks immediately upon a button press, but if we're 
         going from unlocked to locked, then we introduce a delay.
  
         In the video, they mentioned the door waits 3.5 seconds
         before locking - this is why.  delay() takes a number of
         milliseconds, and 3500 ms == 3.5 s
      */
      delay(3500);
    }

    /* lockEngage is a little redundant here - we *could* just set
       each pin to "lockState", since lockState & lockEngage are the
       same once we run this "=" statement.  This is probably more
       leftover stuff from development, testing, debug, etc.
       The variable name "lockEngage" is actually a little misleading
       in this case, because if the door was locked, and we pushed the
       button, then lockState just changed from HIGH to LOW, and when
       we set RELAY_PIN to LOW, we'll actually be *dis*engaging the lock.
       When you write code, you can name variables anything you want -
       it doesn't have to make sense.  Please do choose meaningful names,
       though.  Your coworkers will thank you.
    */
    lockEngage = lockState; 

    /* Set the RELAY and LED pins to the new lockState. */
    digitalWrite(RELAY_PIN, lockEngage);
    digitalWrite(LED_PIN, lockEngage);
  }  
}


/* Normally, with C code, you'd have a function named "main".
   When a program runs, it looks for "main()" and executes it - that's
   the starting point of the whole program.

   For Arduino though, it looks like we start by running "setup()" once,
   then we just repeatedly run the function "loop()".

