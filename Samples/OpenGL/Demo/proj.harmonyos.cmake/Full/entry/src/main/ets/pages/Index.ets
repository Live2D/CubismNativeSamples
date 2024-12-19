/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

import hilog from '@ohos.hilog';
import { WindowManager } from '@kit.SpeechKit';
import live2dDemo from 'libDemo.so';

function moveTaskToBack(): void {
    WindowManager.getWindowStage().getMainWindowSync().minimize();
}

@Entry
@Component
struct Index {
    private xComponentId = 'Demo';

    build() {
        Stack() {
            XComponent({ id: this.xComponentId, type: XComponentType.SURFACE, libraryname: 'Demo' })
            .onLoad(() => {
                hilog.info(0x0000, '[live2dDemo]', 'onLoad');
                live2dDemo.initializeMainLoop(getContext().resourceManager, moveTaskToBack);
            })
            .key('Demo')
            .onDestroy(() => {
                hilog.info(0x0000, '[live2dDemo]', 'onDestroy');
            })
            .width('100%')
            .height('100%')
        }
    }
}
