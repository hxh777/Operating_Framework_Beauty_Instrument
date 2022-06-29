//import QtQuick 2.0
//import QtQuick 2.2
//import QtMultimedia 5.0

//Rectangle {
//     id: page
//     width: 500; height: 200
//     color: "lightgray"

//     Text {
//         id: helloText
//         text: "Hello world!"
//         y: 30
//         anchors.horizontalCenter: page.horizontalCenter
//         font.pointSize: 24; font.bold: true
//     }
// }


import QtQuick 2.4
import QtMultimedia 5.4
import QtQuick.Controls 2.0
import People 1.0







Rectangle {
    id:rect
    width: 1000;
    height: 1000;
    property bool isPlay: true
    property bool isFirst: true
    property bool isFirstclicked: true


    MediaPlayer {
        signal signalA

        id:player;
       // source: "file:///storage/emulated/0/DCIM/Camera/VID_20210905_120504608.mp4";
        source: "qrc:/video/thigh.mp4";
        autoPlay: true
        autoLoad: true

       // volume: 0.5
        onPlaying: {
            if(isFirst)
            {player.playbackRate = 20;
            isFirst = false ;}
            else
            {player.playbackRate=1;
            }
        }
        onStopped: {
            isPlay = false;
        }




        onError: {
            console.log(errorString);
        }


        onSignalA: {

            console.log(player.autoLoad)
            console.log("click 5");
        }


    }



    VideoOutput {
        id:video
        anchors.fill: parent;
        source: player;
    }


    function one(){
        console.log("one");
    }
    function two(string){

        player.pause();
        console.log(string)
        person.walk();
    }




    Person {
        id: person
        onTired: {  // 信号处理程序
            //label.text = "Rest: " +  minute


            player.signalA();


        }

    }

    MouseArea {
        anchors.fill: parent;
        onClicked: {
            console.log("call play");

            //cpp1.signalsMsg("Hello ")
            if(isPlay == true)
            {
                player.pause();
                isPlay = false;
                //playImage.visible = true
            }
            else
            {
                if(isFirstclicked == true)
                {
                    player.stop();
                    player.play();
                    isFirstclicked = false;
                    isPlay = true;
                }
                else
                {
                    player.play();
                    isPlay = true;
                }
                //playImage.visible = false
            }
        }


    }





    Rectangle {
            id: progressBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 0
            height: 70
            color: "lightGray"

            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: player.duration>0?parent.width*player.position/player.duration:0
                color: "darkGreen"
            }

            MouseArea {
                property int pos
                anchors.fill: parent

                onClicked: {
                if (player.seekable)
                    pos = player.duration * mouse.x/width

                    player.seek(pos)
                    three()
                }

                function three(){
                player.pause()
                isPlay=false}
            }
        }

}










