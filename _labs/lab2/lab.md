---
layout: page
title: 'Lab 2: Below C Level'
permalink: /labs/lab2/
released: true
---

*Lab written by Pat Hanrahan*

![Below C Level](images/belowClevel.jpg)

From Death Valley, CA.

### Goals

During this lab you will:

1. Understand the assembly language produced by ```gcc``` when compiling a C program.

2. Understand basic ```Makefile```s.

3. Learn how to unit test your C program.

3. Setup up a 4-digit 7-segment display 
   for your next assignment - building a clock.

We have broken the lab into 4 sections. 

**To complete the lab, you must answer the questions in the
[checklist](checklist) and check with a TA.** We don't grade you on
correctness or collect your answers, but we want to make sure you've
completed the lab and understand the concepts.

### Prelab preparation

To prepare for this lab, you should do the following.

1. Read the [gcc](/guides/gcc) tutorial
   about how to compile C programs for bare metal
   programming on the Raspberry Pi.

2. Read the [make](/guides/make) guide
   on setting up makefiles for cross-development
   on the Pi.

3. Read section 4.1 in this lab on the theory of operation
   for 7-segment displays. Also, skim the rest of section 4.

To start this lab, find the `cs107e.github.io/_labs/lab2/code` directory.
You may need to pull from the `cs107e.github.io` repository
to get the latest code.
(Run `git pull` in your cloned `cs107e.github.io` folder from the previous lab.)


### Lab exercises

#### 1. Compiling C to assembly (20 min)

The goal of this exercise is to understand how C is translated 
into assembly language. You won't have much occasion to hand-code 
gobs of assembly, but you will often spend time __reading__ assembly.

We want you to have an intuitive understanding of 
how the compiler generates assembly from C
and be able to inspect that assembly to better understand 
the execution of a program.
As we will see, sometimes the assembly language
produced by the C compiler can be surprising. 
Using your ARM superpowers, you can dig into the generated 
assembly and figure out what the compiler did, instead of sitting 
there dumbfounded when an executing program does not behave as expected!

Go to the `codegen` directory. Open the `codegen.c` source file in
your text editor. We won't compile this code just yet (we'll use the
online Compiler Explorer soon).

The code is decomposed into a number of functions, each of which explores 
a particular issue for code generation. You will have to wait until
Friday's lecture to hear about the operation of C function call/return, so 
for now, take it on faith that:

- the first four parameters are placed in r0, r1, r2, and r3,
- the return value is to be written to r0, and
- the ARM instructions `bl` and `bx` direct control to/from a subroutine.

The comments in `codegen.c` guide you through a few concepts:

(a) if/else,

(b) loops,

(c) arrays and pointers,

(d) if/else versus switch.

Review the C code and read the comments we left for function.
To see how that C is translated by the compiler, you can run `make` and 
open the `codegen.list` file, but it's a bit faster (and definitely more fun!) to
play with the online Compiler Explorer we used in lecture.

Choose the compiler `ARM gcc 5.4` and enter flags `-Og -ffreestanding
-marm` in the [Compiler Explorer](https://gcc.godbolt.org/). This
gives you a pretty close approximation of our compiler
version/environment (although not an exact match). Paste the parts
from `codegen.c` in, one by one, and work through the generated assembly
to understand the compiler's translation.

During lab time, we'd like you and your partner to work through the
first three parts (a)-(c), and put off the somewhat more involved part (d) 
for another time.  Use the comments in
the C source as your guide. 

Keep in mind that a great way to learn how a system works is by trying
things. Let your curiosity be your guide!

#### 2. Understand Makefiles (15 min)

Break into pairs and read the following Makefile.

```
    NAME = blink

    CFLAGS  = -g -Wall -Og -std=c99 -ffreestanding

    all: $(NAME).bin
     
    %.bin: %.o
        arm-none-eabi-objcopy $< -O binary $@

    %.o: %.c
        arm-none-eabi-gcc $(CFLAGS) -c $< -o $@
    
    %.list: %.o
        arm-none-eabi-objdump -d $< > $@

    install: $(NAME).bin
        rpi-install.py $<

    clean:
        rm -f *.o *.bin *.list
```

Discuss and document all the various features and syntactical
constructs used in this Makefile.

 - What do each of the CFLAGS do?
 - What happens if you just type `make`? Which commands will execute?
 - If you modify blink.c and run `make` again, which commands will rerun?
What part of each target indicates the prerequisites? (A prerequisite means 
that if that file changes, then the target is stale and must be rebuilt)
 - What do the symbols `$<` and `$@` mean?

You should be able to answer the [first checklist question](checklist) now.

#### 3. Testing (15 min)

As you write more complicated programs, you'll want to test
them; keeping track of what parts of the program work and what parts don't is essential
to debugging effectively. Starting with assignment 2, we'll provide you with a few
automated tests and tools you can use to write your own tests.
Let's walk through a simple example with tests.

##### A buggy program

Go to the `testing` directory in your terminal.

Look at the simple program in `testing.c` that defines an `is_odd` function
and then uses the `main` function to test `is_odd` for validity. In particular, notice
the calls to `assert()` made from the `main()` function. The 
expressions we pass to `assert()` should evaluate to true if our program is working
properly. If not, we have a bug.

##### Build the program

Now run `make`.

Running `make` will generate both `testing.bin` and
 `testing.list`. To produce `testing.bin`, the computer needs to
_compile_ `testing.c` to `testing.o`, then _link_ that `.o` with some
other object files to form `testing.elf`, then strip that down to
`testing.bin`. We will learn more about linking later.

(You'll probably want to run `make` once on the original C files, and
then run `make` again if you change the C code, so you can test the
new version of the program.)

Even if the linking step fails and `testing.bin` isn't made, as long
as the compile of the individual C file works, you should still get a
`testing.list` you can look at. A `.list` file contains a listed
representation of the ARM assembly instructions compiled from the
corresponding C source file.

But your `make` just now should have worked (although the program
itself will have a bug!), so we can run the program and watch the bug
in action.

##### What do you expect?

Before we run the program, let's think about what we expect to
happen. The `assert` macro (in `assert.h`) will call `abort` if its 
argument evaluates to false, but what does `abort` do? (hint: look in `abort.c`)

Next, look at `cstart.c` and determine what will happen if your
program returns from `main()` without an assertion failure (i.e., what
will happen if the program works!). Don't worry about the `bss` stuff
for now: we will talk about that in class soon.

__If `is_odd()` has a bug, what would you expect to see on the Pi? In
contrast, what would you expect to see on the Pi if `is_odd()` worked
properly?__


##### Run the program

Run `rpi-install.py testing.bin`. You should get the blinking red
LED of doom. You now know at least one test failed, but which one?
The strategy from here is to iterate, selectively commenting in/out 
test cases and re-running to narrow in on which specific cases fail. 
How many of the test cases pass? How many fail?  Which ones? Why?

Use the information you glean from the test cases to identify what is wrong.
Now fix the bug in `is_odd` so that it works correctly for any argument.
Uncomment all test cases, rebuild, re-run, and bask in the glow of the green light of happiness!

##### Bonus tasks

Phew, typing out `rpi-install.py testing.bin` so many times was
incredibly taxing on your poor fingers! Try add a recipe for
`install` to your Makefile that allows you to build and a run
a test program on your Pi with a one-line command, `make install`.

Oh, one last thing -- you can also try adding a test that you expect to
fail. Think of something that shouldn't work and assert it, and make
sure you get the assertion failure you expected.

#### 4. Setup up a 4-digit 7-segment display (70 min)

Your next assignment will be to build a simple clock
using a 4-digit 7-segment display. This lab will get you
set up to do this.

This lab has been deliberately designed to step you 
through process and to *test as you go*.
We start simple,
test it to make sure you understand how the display works,
and then add more functionality.
For parts 4.2, and 4.3, feel free to use jumpers for ease of debugging!


#### 4.1 How it works

Let's start by understanding how a single 7-segment display works.

![7-segments](images/segments.png) 

The 7-segment display, as its name implies,
is comprised of 7 individually lightable LEDs,
labeled A, B, C, D, E, F, and G.
There is also a decimal point labeled DP.
Each segment is an LED. 
Recall that an LED has an anode and a cathode.
The polarity matters for an LED;
the anode voltage must be positive relative to the
cathode for the LED to be lit.
If the cathode is positive with respect to the anode,
the segment is not lit.

On the 7-segment displays we are using,
the cathodes are all connected together.

![Common Cathode](images/common.cathode.png)

Such a display is called a *common cathode* display.

To create a number, you need to turn on some of the segments.
What segments do you need to turn on to make a '1', a '0', an 'A'?

Here is a
[nice online simulation of a 7-segment display](http://www.uize.com/examples/seven-segment-display.html).

Your clock will display minutes and seconds.
We will need 2 digits for the minutes and 2 digits for the seconds,
for a total of 4 digits.
We will be using a display with four 7-segment displays,
all integrated into a single unit.

[<img src="images/display.jpg" title="4-digit, 7-segment display" width="200">](images/display.jpg)

Here is a more detailed diagram of the package:

[<img title="4-digit, 7-segment display" src="images/display.package.png" width="600">](images/display.package.png)

and of the schematic:

[<img title="4-digit, 7-segment display" src="images/display.schematic.png" width="600">](images/display.schematic.png)

Study these diagrams.
First, notice that there are 12 pins.
There are 4 digit pins,
labeled D1, D2, D3, and D4;
and 8 segment pins labeled A, B, C, D, E, F, G, DP.
Notice how the pins are internally wired to the LEDs.
Each digit is an individual common cathode 7-segment display.
The segments (A-G) is wired to all 4 digits.
And the cathode for each digit has a separate pin.

Here is a handy photo of the display with the pins labeled.

![4-digit, 7-segment display](images/display.labeled3.jpg)

The pins also have numbers.
The pin on the bottom-left is numbered 1,
and they increase as you move right up to 6,
and then continue around on the top.
Note that pin 12 is in the top-left corner.

#### 4.2. Wire up the resistors to the segment pins of the display

Let's wire up the segments of display and turn them on.

First, connect the two power rails and two ground rails. This makes
accessing power and ground via jumper cables more convenient.

![Breadboard with two wires](images/power-rails.jpg)


Second, place the display on the right side of the breadboard.
Make sure the display is oriented correctly
(the decimal points should be on the bottom,
and the digits slanted to the right).
My convention when using a breadboard is to always place 
the blue ground rail on the bottom 
(after all, ground is always underneath us).

**I placed my display so pin 1
is aligned with column 50 on the breadboard.**

That makes it easier to know which numbered hole 
is connected to a pin,
since after you insert the display into the breadboard 
you can't see the pins.

Third, place a single 1K resistor on the board.
Install the resistor so that it crosses over the middle of the breadboard.

Hook up the power and ground rails of the breadboard
to the 3.3V and Ground pins on your Raspberry Pi.
Find three short male-male jumpers.
Wire the top of the resistor to the red power rail 
using an orange jumper (since orange indicates 3.3V),
and the bottom of the resistor to A (Pin 11 - if you aligned pin 1 to
column 50 as described above, this is at column 51) using a green jumper.
Then wire D1 (Pin 12) to Ground using a black jumper.
You may want to refer to the diagram above
that shows the pins of the display labeled.
When you apply power to your Raspberry Pi,
you should see the result shown below.

![Wired breadboard with components](images/jumper1.jpg)

Now experiment.
After you light up segment A of digit 1,
light up segment B of digit 1.
Then rewire it so that you light up segment B of digit 2.
Finally, light up segment A and B of digit 1 **and** 2.
Note that you cannot simultaneously display 
different segments on different digits: Why?

Next, place eight 1K current limiting resistors 
in a row on your breadboard.
Let's always use the convention 
that the left-most resistor controls segment A,
and the right-most controls segment DP.
After you insert the resistors, test your circuit.
Apply power to various segments and create the pattern `"1 1 "`.
Here a space means that the digit is blank (no segments turned on).

![Wired breadboard with components](images/jumper2.jpg)

Figure out how to display other numbers.

#### 4.3. Wire up the transistors to the digit pins

Up to now, you have been turning digits on and off
by grounding the digit pin.
We will eventually want to control which segments and digits 
are turned on using the Raspberry Pi GPIO pins,
so we need an electronic switch that can be controlled using these pins.
To do this we will use bipolar-junction transistors, or BJTs. 

A transistor has 3 terminals— the base (B), collector (C), and emitter (E).
The base controls 
the amount of current flowing from the collector to the emitter.
Normally, no current flows from collector to emitter.
This condition is an open circuit.
However, if you apply 3.3V to the base, 
the collector will be connected to the emitter and current will flow.
This is equivalent to closing the switch.

We will be using 2N3904 transistors.
The following diagram identifies which
pins on the 2N3904 are collector, base, and emitter.

![2n3904](images/2n3904.jpg)

Note the transistors have a flat side and a rounded side.
If you are looking at the flat side with the text upright,
the leftmost leg will be the emitter.

When you wire up a BJT,
you need to use a current limiting resistor
between the base and the control voltage.
Now wire the collector of the left-most transistor to D1
(remember the collector is the right-most pin if the flat
side is facing you).
And apply power to the base of the transistor.
You should see `"1 ‍ ‍ ‍ "` on the display.

Here's a board where we've connected _both_ D1 and D3 to the
collectors of transistors, and then applied power to the bases of
those two transistors, so we see `"1 1 "` on the display.

![Wired breadboard with components](images/jumper3.jpg)

#### 4.4. Permanently wire up the circuit

Now comes the time consuming part.
Connect each resistor to the correct segment of the display.
Then wire the collector of each transistor to the digit on the display.
Be patient, this takes some time.
However, if it's taking you more than half an hour in lab, try moving on and coming back to this part.

Here is a photo of what it should look like before wiring.

![Wired breadboard with components](images/parts.jpg)

And below is what it should look like when it's all wired up, except
for the GPIO pins on the Pi.

In the below diagram, two dots with the same color are connected by a
wire. The GPIO pin numbers are placed where you should connect jumpers
from the pins on the Pi. The white bar down the side indicates row 50
on the breadboard, which is where the left-most pin of the 7-segment
display would be, if you placed it according to our advice earlier in
this lab.

You will see some buttons in the photo. There is no need to add those
buttons during lab.  The buttons are used for the extension to
Assignment 2.

![Wired breadboard with components 2](images/wire2.jpg)

Here's the full breadboard hard-wired to test one segment and digit,
instead of to the Pi.

![Wired breadboard with components 1](images/wire1.jpg)

#### 4.5. Connect the display to the Raspberry Pi

The final step is to connect our display to the Raspberry Pi.
After we connect the Pi to the display, 
we can control the display with a program.
We will outline the process in the next few paragraphs.
However, there is no need to do this in lab.
You should be able to do this on your own outside of lab.

We will use GPIO pins 10-13 to control the 4 digits.
Pin 10 will control the first digit,
pin 11 will control the second digit, and so on.
We will dedicate GPIO pins 20-27 to control 
the 8 segments on the display (A-G and DP).
Pin 20 will control pin A, pin 21 is B, etc.
In total, we will 12 GPIO pins on the Pi:
8 pins to control which segments are turned on,
and then 4 pins to control which digits are turned on.

To create a digit,
your program will set segment GPIO pins high 
to turn on segments you want to light up.
Your program will also select 
which digit to display by turning on the transistor.
When you implement your clock,
you will write a program that will 
first display digit 0,
then it will turn off digit 0 and turn on digit 1, 
and so on.
Add some logic to keep track of time,
and you have a clock!

The extension for this assignment challenges you to provide a UI to the clock
that lets you set the time.
You are constrained to use only 2 buttons.
Here is how we wired up 2 buttons on our breadboard.

![Buttons](images/button.jpg)

Make sure you [check in](checklist) with a TA before you leave.

### Later

After this lab, on your own time, you may try the following:

1. Finish any parts of `codegen.c` you didn't complete during lab.

2. Go through the codegen and testing Makefiles.
