# momotarou
3月大島

## ドキュメント

- [Gitの使い方](./docs/git.md)
- [CAN通信仕様](./docs/can.md)

## ビルド方法
```
cd ビルドするやつ
mbed deploy
mbed compile
```

## ロケット電装

- マイコンは全てSTM32
- ノーズ部とメイン電装(layer1〜3)がある
- layer3は電源と3種の独立したロガー

- CANによるマイコン間通信
	- layer1内部
	- layer2内部
	- ノーズ電装 <---> メイン電装

|場所  |名前|やること|
|------|----|--------|
|ノーズ|nose  |動圧取得|
|layer1|opener|開放機構の動作，EEPROMにログ保存|
|layer1|ES    |コマンド受信，テレメトリ送信|
|layer1|TWE   |テレメトリ送信|
|layer1|SD    |microSDにログ保存|
|layer2|MPU   |加速度・角速度・地磁気取得|
|layer2|BME   |気圧・気温取得|
|layer2|GPS   |GPSデータ取得|
|layer2|press |燃焼室圧力取得|
|layer3|logger1|3軸加速度・3軸角速度・3軸地磁気取得，保存|
|layer3|logger2|3軸加速度取得，保存|
|layer3|logger3|気圧・気温取得，保存|

## バルブ電装

超シンプルなトワ付きスイッチを入れると -> バルブが回り -> うれしい！！！！！！！！
