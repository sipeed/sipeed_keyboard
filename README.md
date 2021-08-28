# sipeed_keyboard
![FirmwareCI](https://github.com/sipeed/sipeed_keyboard/actions/workflows/build_firmware.yml/badge.svg) <br>
sipeed opensource mechanical keyboard make with BL706 <br>
矽π 开源双模机械键盘



## 目录结构
```
./
├── LICENSE
├── README.md
├── firmware // 存放键盘固件
├── hardware // 硬件设计
├── software // 上位机软件
├── teamworks // 贡献记录
└── tools // 工具
```

## 构建键盘固件
1. 参考[开发环境搭建指南](http://bouffalolab.gitee.io/bl_mcu_sdk/get_started/index.html)

```sh
git clone https://github.com/sipeed/sipeed_keyboard.git
git submodule init
git submodule update
cd sipeed_keyboard/firmware/bl_mcu_sdk
make build BOARD=sipeed_keyboard APP=sipeed_keyboard_68 APP_DIR=../keyboard

```
## 交流QQ群
![](https://img.hackx.cc/img/qrcode_1626271263202.jpg)