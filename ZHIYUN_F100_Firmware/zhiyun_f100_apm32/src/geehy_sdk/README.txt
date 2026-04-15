本目录下的 .c / .S 文件为指向 ZHIYUN_F100/SDK 的符号链接，由脚本生成。

在项目根目录执行以下命令刷新链接：
./scripts/link_geehy_sdk.sh

说明：
- `link_geehy_sdk.sh` 仅会清理本目录中的 `.c` / `.S` 链接文件。
- `README.txt` 会被保留，便于克隆后定位前置步骤。
本目录下的 .c / .S 为指向 ZHIYUN_F100/SDK 的符号链接，由脚本生成（不提交到 git 时请忽略）。
在项目根目录执行: ./scripts/link_geehy_sdk.sh
