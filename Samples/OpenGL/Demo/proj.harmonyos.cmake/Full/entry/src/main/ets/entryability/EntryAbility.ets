/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import hilog from '@ohos.hilog';
import UIAbility from '@ohos.app.ability.UIAbility';
import Want from '@ohos.app.ability.Want';
import window from '@ohos.window';
import { WindowManager } from '@kit.SpeechKit';

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
      hilog.info(0x0000, '[live2dDemo]', '%{public}s', 'Ability onCreate');
  }

  onDestroy(): void {
      hilog.info(0x0000, '[live2dDemo]', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
      // Main window is created, set main page for this ability
      hilog.info(0x0000, '[live2dDemo]', '%{public}s', 'Ability onWindowStageCreate');

      WindowManager.setWindowStage(windowStage);
      windowStage.loadContent('pages/Index', (err, data) => {
          if (err.code) {
              hilog.error(0x0000, '[live2dDemo]', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
              return;
          }
          hilog.info(0x0000, '[live2dDemo]', 'Succeeded in loading the content. Data: %{public}s', JSON.stringify(data) ?? '');
      });
  }

  onWindowStageDestroy(): void {
      // Main window is destroyed, release UI related resources
      hilog.info(0x0000, '[live2dDemo]', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
      // Ability has brought to foreground
      hilog.info(0x0000, '[live2dDemo]', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
      // Ability has back to background
      hilog.info(0x0000, '[live2dDemo]', '%{public}s', 'Ability onBackground');
  }
};
