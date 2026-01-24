import QtQuick 2.0

Text {
    id: scrollingText
    property int fontSize: 30
    property string fontFamily: "uncage"
    property bool leftAlign: true

    font.pixelSize: fontSize
    font.family: fontFamily
    width: parent.width
    height: parent.height
    verticalAlignment: Text.AlignVCenter

    SequentialAnimation {
        id: scrollAnim
        loops: Animation.Infinite
        running: false

        PauseAnimation { duration: 1500 } // Пауза 1,5 секунды в начале
        NumberAnimation {
            target: scrollingText
            property: "x"
            from: 0
            to: -scrollingText.contentWidth + scrollingText.width
            duration: scrollingText.contentWidth > scrollingText.width ? (scrollingText.contentWidth - scrollingText.width) * 14 : 0
            easing.type: Easing.Linear
        }
        PauseAnimation { duration: 1500 } // Пауза 1,5 секунды в конце
        // Сброс позиции текста в конце цикла
        PropertyAction {
            target: scrollingText
            property: "x"
            value: 0
        }
        onStarted: {
            console.log("Анимация началась для", scrollingText.objectName, "from:", from, "to:", to, "duration:", duration);
        }
        onStopped: {
            console.log("Анимация остановлена для", scrollingText.objectName);
        }
        onRunningChanged: {
            if (!running) {
                scrollingText.x = 0;
                if (leftAlign){
                    scrollingText.horizontalAlignment = Text.AlignHLeft;
                }
                else{
                    scrollingText.horizontalAlignment = Text.AlignHCenter;
                }
            } else {
                scrollingText.horizontalAlignment = Text.AlignLeft;
            }
        }
    }

    onTextChanged: {
        console.log("scrollingText", objectName, "contentWidth:", contentWidth, "width:", width, "text:", text);
        if (contentWidth > width && text !== "") {
            scrollingText.horizontalAlignment = Text.AlignLeft;
            scrollAnim.running = true;
        } else {
            scrollAnim.running = false;
            scrollingText.x = 0;
            if (leftAlign){
                scrollingText.horizontalAlignment = Text.AlignHLeft;
            }
            else{
                scrollingText.horizontalAlignment = Text.AlignHCenter;
            }
        }
    }

    Component.onCompleted: {
        console.log("scrollingText", objectName, "contentWidth:", contentWidth, "width:", width, "text:", text);
        if (contentWidth > width && text !== "") {
            scrollingText.horizontalAlignment = Text.AlignLeft;
            scrollAnim.running = true;
        }
    }
}
