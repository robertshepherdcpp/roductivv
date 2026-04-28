# roductivv
my third project for fallout

**what is this project**
- this project is a tool used to help people become productive (roductivv)!
- where is gamifies doing tasks like coding, studying and then punishes activities like gaming and stuyding!
- the different activities are personalized towards me, you can always treat them to be different activities
- it gamifies these tasks through a points based system, where when doing activities like coding you get points, and when doing gaming/scrolling you loose points
- then through gaining points you get to different milstones (like 100/250/500/10000) etc..
- this is all done through a moveable (case on wheels) that sits on your desk and plays different tunes
- the points work like: you add 10 minutes to every task (you can add more by clicking the button more than once) and points are given/taken per 10 minutes.
- studying and coding are uncapped! whilst gaming is capped at 30 minutes a day, and scrolling is capped at 10 minutes a day!
- this project aims to reduce procrastination and boost productivity!!!

**Inspiration/Motivation for building this project**
- i always find myself procrastinating when doing tasks, i know there are apps to do this, but then my phone becomes the distraction and i immediately go onto reels
- so i thought it would be good if i had a physical thing on my desk that could keep me focus
- it also gamifies doing work and punishes doing unproductive stuff (whilst also capping it)
- the aim is to make working addictive (just like the aim of the apps) and should stop my procastination.

**process of making the project**
- pcb was designed in Kicad with components needed usually imported from the lcsc website via an EasyEDA converter tool
- i.e. both schematic and wiring done here in kicad
- the case was made in Fusion360, a simple case with a revolved base so easier to use and less likely to fall over.
- then the axles and wheels where also made in fusion

**how to use**
- first get all the pcb and CAD stuff printed (2 sets of axes and wheels need to be printed)
- then put the axle through the hole made in the case and attach the wheels
- do the same for the other hole
- then insert the case
- screw the lid on (using the bolts as shown in the bom)
- then before flashing to the esp, you need to change the SSID and password in the .ino file to be your own, so that the esp can tell the time (synced with wifi)
- then flash (easiest with the arduino ide), and keeped plugged in and then run!
- there are no batteries, so the connected with the esp must be kept.

**about the firmware**
- done using c++
- to keep track of time (for the timer and also for making sure its only one scroll/3 games a day) i used WiFi so that has to be enabled, that needs to be setup with your own network (ssid and the password)
- then data is stored (e.g. scrolls today, games each day), so we can store the last date scrolled and if it was >= 1 day then we reset the scroll count so u can scroll again. the same is done with gaming.

**screenshots of the project**
the 3d design:
<img width="1134" height="739" alt="image" src="https://github.com/user-attachments/assets/9a39faaf-869c-4e03-84c9-009638c686b7" />
<img width="1107" height="585" alt="image" src="https://github.com/user-attachments/assets/7273f820-94bb-42bb-8d45-35660eb26e5a" />

the schematic:
<img width="1117" height="925" alt="image" src="https://github.com/user-attachments/assets/0d2eabc2-5a81-42c4-99eb-decba9bff97c" />

the footprint:
<img width="948" height="434" alt="image" src="https://github.com/user-attachments/assets/5f381e5a-d4b3-458c-a3ae-49cd32654ede" />

the exploded view:
<img width="803" height="785" alt="image" src="https://github.com/user-attachments/assets/83836775-876b-4ff5-b7ba-0cbf4639f249" />

Zine (pdf can also be found in this repository):
<img width="392" height="586" alt="image" src="https://github.com/user-attachments/assets/96fef559-6f77-48c3-9940-2a8169087ade" />

- Total cost of this project: $40.20. Which was more expensive than i was expecting.
- All the costs can be seen in the BOM.csv (in this repository)

**design considerations**
- i originally wanted to add some wheels, which i 3d printed, however i decided eventually against that, as it will be plugged in via a usb cable anyway.
- however i decided to leave in the holes for the possibility of the user adding their own wheels if they wanted.
