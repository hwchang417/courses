# Object-oriented analysis and design (OOAD)

開發一個UML編輯器，支援以下功能:

## UI方面
* Select: 選取圖形，被選取到的圖形會變色
* Association line: 直線
* Generalization line: 直線(三角形結尾)
* Composition line: 直線(菱形結尾)
* Class: 長方形，中間有兩條橫線
* Use case: 橢圓形

## 圖形細節
* 用多型來描述圖形class的繼承關係，使得UML編輯器可以用基底class來管理所有圖形
* 每個圖形有深度，越早建立的深度越深，在圖形上顯示則是淺層在上，深層在下


## Graph
* 每個圖形提供幾個函式
* onDraw: 繪畫每個圖形
* isInside: 圖形是否在給定的左上右下座標內
* isOutside: 座標是否在圖形外面
* showConnectedPort: 在圖形中心的上下左右四個點分別繪畫小正方形
* isInsideConnectPoint: 判斷座標是位於圖形的上下左右哪個部分，回傳方向

## Menu
* Change Name: 改變圖形顯示的文字
* Group/Ungroup: 將當前選取的物品群組化或去群組化

## GUI
* UML_Canvas: 負責繪畫所有的方格和元件，以及處理觸發事件
* UML_editor: 用來maintain所有的Graph物件和視窗介面

## Button
存放每個物件的觸發事件

## API
存放額外使用到的函式庫，e.g. sort function


