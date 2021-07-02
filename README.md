
# MidiKeyConv

MIDI入力をキー入力に変換するソフトです。

This software converts MIDI input to key input.

![2021-07-03_03h22_21](https://user-images.githubusercontent.com/6138691/124314927-1bac4800-dbae-11eb-819f-d5cd7692fed4.png)

# 使い方 / Usage

1. MIDIデバイスを選択 / Select a MIDI device.

![2021-07-03_03h22_21_2](https://user-images.githubusercontent.com/6138691/124315161-7e9ddf00-dbae-11eb-9f4b-1c4dd1c11404.png)

2. assignModeCheckをクリックすると、キーの登録が出来ます。 / Click assignModeCheck to register the key.

![2021-07-03_03h30_41](https://user-images.githubusercontent.com/6138691/124315659-392de180-dbaf-11eb-9fee-4c04c1554234.png)

3. キーの登録を解除するには右クリックしてください。 / To unregister the key, right-click on it.

# その他機能 / Other Features

## CCへのキーアサイン / Key Assignment to CC

![2021-07-03_03h34_50](https://user-images.githubusercontent.com/6138691/124316076-fa4c5b80-dbaf-11eb-9baa-357a8d96b1cb.png)

* (1) チェックを入れると最後に入力したCCを検出します。 / If checked, the last CC entered will be detected.
* (2) CCを選択します。 / Select CC.
* (3) 選択されたCCのキーシーケンスの方法を指定します。 / Specifies the method of key sequencing for the selected CC.
* (4) CCに登録されているキーです。 / This is the key that is registered in the CC.

## キーシーケンス / key sequencing

### List 


![2021-07-03_03h47_28](https://user-images.githubusercontent.com/6138691/124317039-7dba7c80-dbb1-11eb-8249-20cb7be5bf3b.png)

CC64などのペダル向けの機能です。 127が入力されると登録したキーを出力します。続けて出力する場合はCCの値が一度0になる必要があります。光ってるのは次に出力されるキーです。

This function is for pedals such as CC64. When 127 is input, the registered key is output. To continue outputting, the value of CC must be set to 0 once. The glowing key is the next key to be output.

### Range

![2021-07-03_03h51_29](https://user-images.githubusercontent.com/6138691/124317441-279a0900-dbb2-11eb-8ed9-90c38b609865.gif)

入力したキーを0-127に収まるようにいい感じに配置し、CCの値に応じてキーを出力します。

It places the input keys in a nice way to fit in 0-127, and outputs the keys according to the CC values.
