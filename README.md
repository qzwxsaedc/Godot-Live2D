# GDLive2D
## 1. 使用方法
*参照 [https://docs.godotengine.org/zh_CN/stable/tutorials/scripting/gdnative/gdnative_c_example.html#creating-the-gdnativelibrary-gdnlib-file]()*
1. 将dll复制到项目目录的任意位置
2. 单击`检查器`中的`创建资源`按钮，选择 `GDNativeLibrary`
3. 在下方的docker中找到`Windows`，然后添加一个64位库
4. 通过检查器中的保存按钮**保存**`GDNativeLibrary`(这一步很重要，如果没有保存成功的话下一步会提示找不到对应平台的库)
5. 创建一个`NativeScript`脚本，填写类名为`GDLive2D`，保存
6. 在`NativeScript`脚本的检查器里面设置库(可以直接将第四步创建的`GDNativeLibrary`拖过去)
7. 创建一个`Node2D`节点，挂载第6步创建的脚本。如果前面步骤没有问题的话，`Node2D`的检查器里面应该能看到多了一些设置项、
   1. 第一项(`Root Path`)是模型文件夹的根目录
   2. 第二项(`Model Name`)是模型的名称，要求模型名称=模型目录名称=模型的.model3.json文件名
   3. 剩下的5个是shader，填充官方示例的同名shader即可。
8. 创建一个相机，将缩放设置为5，并勾选`Current`。
9. 按下`F5`

## 2. 播放动作和表情
脚本暴露了四个方法用于播放动作和表情。
### 1. _on_play_motion
函数原型: `void _on_play_motion(group: str, number: int, priority: int)`
- group: 动作组，一般`Idle`是肯定有的
- number: 动作编号
- priority: 优先级，见下
### 2. _on_play_random_motion
函数原型: `void _on_play_random_motion(group: str, priority: int)`
- group: 动作组
- priority: 优先级，见下
### 3. _on_play_expression
函数原型: `void _on_play_expression(id: str, priority: int)`
- id: 表情id
- priority: 优先级，见下
### 4. _on_play_random_expression
函数原型: `void _on_play_random_expression(priority: int)`
- priority: 优先级，见下

### 优先级定义：
- 0: PriorityNone
- 1: PriorityIdle
- 2: PriorityNormal
- 3: PriorityForce

序号小的会被需要大的中断，如果需要强行插入一个动作或者表情的话可以设置为PriorityForce

在没有任何动作输入的情况下，会循环播放Idle动画

# 3. 编译
编译该项目需要:
- msvc >= 140
- cmake >= 3.10
- python >= 3.5
- scons

## 步骤
1. 打开终端，进入项目根目录并输入`git submodule update --init --recursive`。这一步是为了拉取godot-cpp。由于对CubismSDK进行了部分裁剪(删除了Renderer部分)，因此不需要下载CubismSDK。同时为了方便，也包含了CubismCore的.lib。
2. 在终端输入`cd libs\godot-cpp`，再输入`scons p=windows bits=64 generate_bindings=yes target=release -j6`编译godot-cpp。编译完成后回到项目根目录
3. 在终端输入`mkdir build`，或直接在项目根目录新建一个文件夹用于编译
4. 在终端输入`cd build`，或进入新建好的目录打开终端
5. 输入`cmake ..`，等待cmake创建Makefile
6. 输入`cmake --build . --config Release`进行编译。编译好的dll在`Release`(Release模式)或`Debug`(Debug模式)下
## 注意事项
1. 需要注意godot-cpp和项目的编译模式要统一，不要一个用release编译另一个用debug模式编译。
2. godot用的是64位，因此项目强制指定了64位模式编译，因此godot-cpp也需要生成64位的静态库。也可以自行修改CmakeLists.txt修改位数，记得生成对应位数的godot-cpp，并修改cubism的静态库为对应的位数。
3. 默认以release模式编译。
4. 用vsBuildTool可能会出现无法编译的问题。请自行打开`x64 Native Tools Command Prompt for VS 20xx`用`nmake`或者`msbuild`来编译。

# 4. 已知问题
1. 部分依赖Mask来实现的功能会出问题。例如，眨眼的时候会出现眼睛没有被正确遮挡。**短期内不会解决(因为没搞懂官方示例是怎么做的)**
2. 没有做光标跟随。**近期如果有空再考虑**
3. 没有做TapBody的功能。**这个可以靠外置一个CollisionPolygon2D来做。有空再考虑内置**