#!/bin/bash
clear
cd src
(make -k cara && cd .. && cd obj && ./cara ) || cd ..
