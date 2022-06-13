# Pawapuro-TextConverter
PS2初期の実況パワフルプロ野球におけるゲーム内文字コード（パワプロ文字コード）を、シフトJISと相互変換するツール

-----
## 対象作品(いずれもPS2版)
- 実況パワフルプロ野球7(決定版)
- 実況パワフルプロ野球8(決定版)
- 実況パワフルプロ野球9(決定版)
- 実況パワフルプロ野球10(超決定版)
- 実況パワフルプロ野球11(超決定版)
- 実況パワフルプロ野球12(決定版)
- 実況パワフルプロ野球2009

-----
#動作環境・注意点
- 想定動作環境：Windowsのコマンドプロンプト
    - 対話モードでの正しい表示にはシフトJIS(CP932)での入出力が必要

- シフトJIS→パワプロ文字コードモードの時、全角文字を入力すること

-----
# コマンド
### `Pawapuro-TextConverter.exe [FILE] [OPTION]`

- コマンドライン引数無し
    - 対話モードでどの変換をするか選ぶ
        - シフトJIS→パワプロ文字コード
        - パワプロ文字コード→シフトJIS


### **[FILE]** 
指定するとファイルを読み込み、パワプロ文字コード→シフトJISに変換する

### **[OPTION]**

| OPTION | 説明 |
| ---- | ---- |
|**ターゲット作品**||
|-t TARGET<br>--target TARGET<br>-g TARGET<br>--game TARGET|対象のパワプロを指定|
|**ターゲットゲームモード**||
|-m GAMEMODE<br>--mode GAMEMODE|ゲームモードを指定<br>サクセス用コマンドを文字列に置き換える<br>`normal` または `success`|
|**コマンド表示レベル(サクセス指定時)**||
| -v0 | 改行、`[主人公]`のみ |
| -v1 | 通常レベル |
| -v2 | 詳細表示 |
|**その他**||
|-h<br>--help|ヘルプの表示|

### TARGET用コマンド

| コマンド | 指定する作品 |
| ---- | ---- |
|7 pawa7<br>7k pawa7k| 実況パワフルプロ野球7(決定版) |
|8 pawa8<br>8k pawa8k| 実況パワフルプロ野球8(決定版) |
|9 pawa9<br>9k pawa9k| 実況パワフルプロ野球9(決定版) |
|10 pawa10<br>10k pawa10k| 実況パワフルプロ野球10(超決定版) |
|11 pawa11<br>11k pawa11k| 実況パワフルプロ野球11(超決定版) |
|12 pawa12<br>12k pawa12k| 実況パワフルプロ野球12(決定版) |
|2009 pawa2009| 実況パワフルプロ野球2009 |

-----
# ToDo

- [ ] テキストデータが圧縮されている作品での復号化
    - サクセスモードのシナリオテキストはパワプロ7,8では圧縮されていない(SUCCESS.BIN/SCS_MAIN.BINに格納されている)
    - 9以降ではサクセスのテキストが圧縮されている
    - 1文字に12ビットあれば充分で、4文字分が6バイト(48ビット)に圧縮されている。
    - ターゲットの作品をパワプロ9以降に、ゲームモードをサクセスに指定すると、復号化処理を行う
- [ ] パワプロ9以降のコマンド解釈

- [x] パワプロ7の文字コードへの対応
    - パワプロ8以降とはコードが違う　文字集合は基本的にJIS X 0208
    - サクセス用コマンドがパワプロ8とは違う