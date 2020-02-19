# CANの通信仕様について

話し合いなどは #52 で．

- 通信速度: 1Mbps

IDとデータ種の対応は以下．

| ID |データ種類|送信マイコン|受信マイコン|
|----|----------|------------|------------|
|0x01|コマンド  |ES          |opener,SD   |
|0x02|エラー    |任意        |opener,ES,TWE,SD|
|0x03|気圧      |BME         |opener,ES,TWE,SD|
|0x04|合成加速度|MPU         |opener,TWE|
|0x05|GPS       |GPS         |ES,TWE,SD|
|0x06|フェーズ  |opener      |ES,TWE,SD|
|0x07|姿勢角    |MPU         |TWE,SD|
|0x08|3軸加速度 |MPU         |TWE,SD|
|0x09|3軸角速度 |MPU         |TWE,SD|
|0x0A|燃焼室圧力|圧力センサ  |TWE,SD|
|0x0B|動圧      |ノーズ      |TWE,SD|
|0x0C|気温      |BME         |TWE,SD|
|0x0D|地磁気    |MPU         |TWE,SD|
