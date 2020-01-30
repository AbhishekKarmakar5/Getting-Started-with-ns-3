# Getting-Started-with-ns-3
https://www.nsnam.org/docs/tutorial/html/

This is the ns-3 Tutorial. Primary documentation for the ns-3 project is available in five forms:

ns-3 Doxygen: Documentation of the public APIs of the simulator
Tutorial (this document), Manual, and Model Library for the latest release and development tree
ns-3 wiki
ns-3 Tutorial

This document is written in reStructuredText for Sphinx and is maintained in the doc/tutorial directory of ns-3’s source code.

    Introduction
        About ns-3
        For ns-2 Users
        Contributing
        Tutorial Organization
    Resources
        The Web
        Git
        Waf
        Development Environment
        Socket Programming
    Getting Started
        Overview
        Prerequisites
        Downloading ns-3 using Git
        Building ns-3
        Testing ns-3
        Running a Script
    Conceptual Overview
        Key Abstractions
        A First ns-3 Script
        Ns-3 Source Code
    Tweaking
        Using the Logging Module
        Using Command Line Arguments
        Using the Tracing System
    Building Topologies
        Building a Bus Network Topology
        Models, Attributes and Reality
        Building a Wireless Network Topology
        Queues in ns-3
    Tracing
        Background
        Overview
        Real Example
        Trace Helpers
        Summary
    Data Collection
        Motivation
        Example Code
        GnuplotHelper
        Supported Trace Types
        FileHelper
        Summary
    Conclusion
        Futures
        Closing

# Running scripts

    1 Running C++ scripts
    2 Running python scripts
    3 Running scripts in a shell
    4 Running C++ scripts with gdb

# Running C++ scripts

Once the build is done and all tests pass, one generally wants to run a couple of examples as per the tutorial. The easiest way to do this is to run:

./waf --run progname

To find out the list of available programs, you can do:

./waf --run non-existent-program-name.

Running python scripts

The method to run a python script is fairly similar to that of a C++ script except that you need to specify a complete path to the script file, and you need to use the –pyrun command instead of –run. For example:

./waf --pyrun examples/wireless/mixed-wireless.py

Running scripts in a shell

Another way to run ns-3 programs that does not require using the ./waf –run command is to use the ns-3 shell which takes care of setting up all the environment variables necessary to do so:

./waf shell

And, then:

./build/debug/examples/csma-broadcast

Running C++ scripts with gdb

If you do not use the ns-3 shell, and if you want to run your simulation under a special tool (valgrind or gdb), you need to use a command template:

./waf --run csma-cd-one-subnet --command-template="gdb %s"

or:

./waf --run csma-cd-one-subnet --command-template="valgrind %s"

