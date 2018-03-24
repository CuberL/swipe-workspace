# swipe-workspace

This is a program that can use the touchpad to switch workspaces, base on libinput.

The test environment below is Linux Mint 18.1 Cinnamon 64-bit, on my Lenovo Y510p laptop.

![](http://7tebhf.com1.z0.glb.clouddn.com/output1.gif)

## Dependencies

The program is written in C and base on [libinput](https://wayland.freedesktop.org/libinput/doc/latest/), it use [xdotool](https://github.com/jordansissel/xdotool) to switch workspaces. So you need gcc, xdotool and libinput to compile it.

```
$ sudo apt-get install gcc libinput-dev xdotool
```

## Compile

```
$ gcc main.c -o swipe_workspace -linput
```

And add it into you PATH.

## Usage 

```
$ swipe_workspace
```

## Something may need to adjust

Because of the difference with touchpad and usage habit, you may need to edit the code by yourself. There is two constant in the code: INTERVAL, means the shortest interval between two switch actions; and another one is the THRESHOLD, if you found it is too sensitive, you can increase it.

By the way, Linux Mint Cinnamon use `ctrl+alt+←` and `ctrl+alt+→` to switch the workspace, you may need to change the code if you use other operating system.
