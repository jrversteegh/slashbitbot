.. _microbit_slashbot:

BBC micro:bit slashbot
######################

Overview
********
Robot code for BBC microbit + kitronik Compact Robotics Board.

Building
********

This project drives the robot. It can be built as follows:

.. zephyr-app-commands::
   :zephyr-app: slashbitbot
   :board: bbc_microbit_v2
   :shield: kitronik_cbr_5693
   :goals: build flash
   :compact:
