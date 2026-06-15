# DelayEffects 基于信号延迟制作的延迟和镶边音频效果器插件

## 截图
![Delay](./screenshots/Screenshot%20from%202026-04-15%2006-40-59.png)
![Flanger](./screenshots/Screenshot%20from%202026-04-15%2006-40-36.png)

## 插件简介
这是一款用C++和JUCE框架编写的音频效果器插件，主要通过对延迟信号进行不同的播放处理实现不一样的声音效果，目前有delay（延迟）和flanger（镶边）两种模式。延迟效果器的设计比较简单，只有delay time（延迟时间）和feedback（回馈量）这两个控制。镶边效果器的可调参数相对多一些，除了延迟时间外还可以控制LFO的频率、波动深度以及左右声道上相位偏移的大小（可以让声音变得更宽），并且还能选择开启或关闭回馈路径，并调整回馈量的大小。经过处理的信号可以通过mix调整与原信号的比例，并在最后控制输出增益。

## 已知问题
Audio artefacts caused by flanger feedback