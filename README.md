[![License](https://img.shields.io/badge/License-BSD--3-brightgreen.svg)](https://opensource.org/licenses/BSD-3-Clause)

<p align="center">
  <a href="https://github.com/hhijazi/ACOPF">
    <img src="https://github.com/hhijazi/ACOPF/blob/master/logo.svg" width="320" alt="ACOPF">
  </a>
</p>

## About

OPOMO is a C++ tool for solving the [Optimal Power Flow problem](https://arxiv.org/abs/1910.03716) (AC/DC). 
It is built on top of the modeling language [Gravity](https://github.com/coin-or/Gravity), and uses the nonlinear solver [Ipopt](https://github.com/coin-or/Ipopt).

## Citing
Bibtex [ref](https://doi.org/10.48550/arxiv.2203.11328):
`@article{OPOMO,
  title={Benchmarking Large-Scale ACOPF Solutions and Optimality Bounds},
  author={Smitha Gopinath and Hassan L. Hijazi},
  journal={Proceedings of the 2022 IEEE Power Engineering Society (PES) General Meeting (GM)},
  year={2022}
}`

------------------
Numerical Results
------------------
OPOMO is benchmarked against [Matpower](https://matpower.org), [PowerModels](https://github.com/lanl-ansi/PowerModels.jl) and [GridOpt](https://github.com/ttinoco/GRIDOPT) on [PGLiB v21.07](https://github.com/power-grid-lib/pglib-opf/releases/tag/v21.07).
![Benchmarking](https://static.wixstatic.com/media/c6cff5_36a4e63974a240d2861b3af10888ec04~mv2.png)
![Performance Profile on ACOPF](https://static.wixstatic.com/media/c6cff5_96595dc0cbbd4472bde31d664176a5ef~mv2.png)

------------------
Getting Started
------------------

OPOMO has been only tested on Linux and Mac, Windows support is upcoming.

Dependencies
------------

Install [Cmake](http://www.cmake.org) version 3.16 or later.

OPOMO will pull [Gravity](https://github.com/coin-or/Gravity/tree/ACOPF), [Ipopt](https://github.com/coin-or/Ipopt), and [cpp_option_parser](github.com/Lcressot/cpp_option_parser) automatically, you just need to enter the following:

`mkdir build && cd build && cmake .. && make`

The executable will appear under `OPOMO/bin/Release`

Run using: `OPOMO/bin/Release/acopf -f path_to_m_file`

Command Line Options
--


Short_name   | Long_name    | Description | Default_value
:-------------------------:|:-------------------------:|:-------------------------:|:-------------------------:
-h       |     --help   |    shows option help                                                 |    -
-f       |     --file   |    Input file name     |   ../data_sets/Power/nesta_case5_pjm.m
-l       |     --log    |    Log level                                           |    0
-s       |     --scale  |    Scale objective function and thermal limit constraints |    1e-3
-t       |     --tol    |    solver tolerance                                      |    1e-6
-m       |     --model  |    power flow model: ACPOL/ACRECT                    |    ACPOL




Notes
---
Logo constructed using [svg3d](https://github.com/prideout/svg3d), thanks to Philip Rideout.
