# swipe-workspace

一个可以用触控板切换工作区的程序,基于libinput实现。

以下文档使用测试环境均为Linux Mint 18.1 Cinnamon 64-bit，机器为Lenovo Y510P。

## 依赖

程序基于[libinput](https://wayland.freedesktop.org/libinput/doc/latest/)，使用C语言开发，其中切换工作区还用到了[xdotool](https://github.com/jordansissel/xdotool)来模拟按键，所以编译需要有gcc、xdotool以及libinput库：

```
$ sudo apt-get install gcc libinput-dev xdotool
```

## 编译

直接编译即可
```
$ gcc main.c -o swipe_workspace -linput
```

然后可以自行建立软链到/usr/bin目录下。

## 使用 

```
$ swipe_workspace
```

## 参数调节

大家使用的触控板和使用习惯不同，可能需要对源码做少量修改，源码中有两个常量：一个是INTERVAL，指的是两次动作触发最短时间间隔；另一个是THRESHOLD，这个是一个触发的阈值，如果触发太灵敏，可以试着调高，如果觉得触发太迟钝，可以试着调低。
