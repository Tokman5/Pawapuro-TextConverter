# Pawapuro-TextConverter
PS2初期の実況パワフルプロ野球におけるゲーム内文字コード（パワプロ文字コード）を、シフトJISと相互変換するツール

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
|-t TARGET<br>--target TARGET|対象のパワプロを指定<br>指定例: `pawa8` `pawa12k`|
|**ターゲットゲームモード**||
|-g GAMEMODE<br>--gamemode GAMEMODE|ゲームモードを指定<br>コマンド解釈が変わる<br>`normal` または `success`|
|**コマンド表示レベル**||
| -v0 | 改行、`[主人公]`のみ |
| -v1 | 通常レベル |
| -v2 | 詳細表示 |
|**その他**||
|-h<br>--help|ヘルプの表示|


-----
# ToDo
- [ ] テキストデータが暗号化or圧縮されている作品での復号化