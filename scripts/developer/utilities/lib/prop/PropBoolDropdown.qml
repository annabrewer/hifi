//
//  PropBoolDropdown.qml
//
//  Created by Anna Brewer on 8/8/19
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or https://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.7
import QtQuick.Controls 2.2

PropItem {
    Global { id: global }
    id: root

    property alias enums : valueCombo.model

    property var value : false

    PropComboBox {
        id: valueCombo

        flat: true

        anchors.left: root.splitter.right
        anchors.right: root.right 
        anchors.verticalCenter: root.verticalCenter
        height: global.slimHeight

        currentIndex: root.valueVarGetter()
        onCurrentIndexChanged: { root.valueVarSetter(currentIndex); value = (currentIndex == 1); }
    }    
}
