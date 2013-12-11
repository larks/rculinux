# Makefile for an A2F-Linux project

# This project's name
SAMPLE		:= rculinux

# Custom applcations subdirectories that this project
# needs to build. Edit this list as appropriate for your application.
CUSTOM_APPS	:= app app_src

# Include Make rules applicable to all projects
include ../Rules.make
