# CustomToolbox
AviUtl ExEdit2用のプラグインです。エフェクトをタイムラインにドラッグ&ドロップで追加できます。

## 使用ライブラリ
- KENくん氏のAviUtl ExEdit2 Plugin SDK([AviUtlのお部屋](https://spring-fragrance.mints.ne.jp/aviutl/)からダウンロードできます)
- nlohmann氏の[jsonライブラリ](https://github.com/nlohmann/json/tree/develop)

を使用しています。偉大な先人に感謝。

## 導入方法
Releasesから最新のzipファイルをダウンロードし、解凍してCustomToolbox.aux2をプラグインフォルダに配置してください。

## 使用方法
- アイコンをドラッグして、タイムライン上にドロップするとエフェクトが追加されます。タイムライン上の追加したい位置をクリックした後、アイコンをクリックしても追加することができます。
- 設定画面から、アイコンの並び替え、表示・非表示の設定、アイコンの大きさの変更を行うことができます。
- 並び替えを行う際は、設定画面からエフェクトの一つを選択し、青くなった状態で上に移動、下に移動のどちらかを押してください。一度動かすと青色は解除されますが、そのままどちらかのボタンを押せば、並び替えを続けることができます。

## 開発方法
Visual Studioをインストールし、編集します。外部ライブラリは、CustomToolbox/include/ThirdPartyフォルダに配置してください。

## ビルド方法
Visual Studioをインストールし、ビルドします。
