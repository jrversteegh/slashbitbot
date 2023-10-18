.. _microbit_slashbitbot:

BBC micro:bit slashbitbot
#########################

Overview
********
Robot code for BBC microbit + kitronik Compact Robotics Board.

Building
********

This project drives the robot. It requires Zephyr to build.
After installing Zephyr, this project can be built as follows:

``west build -b bbc_microbit[_v2]``

Flashed with:

``west flash``

And debugged with:

``west debug``

.. zephyr-app-commands::
   :zephyr-app: slashbitbot
   :board: bbc_microbit_v2
   :shield: kitronik_cbr_5693
   :goals: build flash
   :compact:
