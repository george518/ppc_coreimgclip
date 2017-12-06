PPC_CoreImgClip
====
啥东西？What?
----
PPC_CoreImgClip是一个php5扩展，他可以获取纯色底图中的核心图，并生成自定义尺寸和边距的新图片。

有什么价值？
----
1、处理图片，jpg,jpeg,png格式的图片       
2、可做为php开发扩展的学习使用 
3、soft文件里是C++实现图片剪切功能代码，可以编译后直接使用 
另外，还有GO和python相关版本，请移步：
Python: https://github.com/george518/CoreImage    
golang: https://github.com/george518/PPGo_ClipImg   <br>
<br />
感觉不错，求star :)
<br /> 
    
效果展示
----
原图<br/>
![github](https://github.com/george518/ppc_coreimgclip/blob/master/images/bags.jpg?raw=true "github")
<br/><br/>
处理图<br/>
![github](https://github.com/george518/ppc_coreimgclip/blob/master/images/newbags.jpg?raw=true "github")

----
## 安装步骤：

### 源码下载

* opencv下载(建议opencv2.4.4版本) ：
    * https://github.com/opencv/opencv/archive/2.4.9.tar.gz
* ppc_CoreImgClip下载：
    * git clone https://github.com/george518/ppc_coreimgclip.git

### 安装`opencv2`

此扩展依赖于`opencv2.0`之上版本。因此安装前先安装`opencv`。

> 步骤如下：

* **安装依赖**

    ```bash
    yum -y install gtk+ gtk+-devel pkgconfig libpng zlib libjpeg libtiff cmake
    ```

* **下载安装 opencv2**

    ```bash
    # 下载
    wget https://github.com/opencv/opencv/archive/2.4.9.tar.gz
    
    # 解压安装包
    tar xvf 2.4.9.tar.gz
    
    # 进入opencv目录
    cd opencv-2.4.9/
    
    # 编译安装
    cmake CMakeLists.txt
    make -j $(cat /proc/cpuinfo|grep processor|wc -l)
    make install
    ```
    
* **设置环境变量**

    ```bash
    cp -p /etc/profile{,.$(date +%F_%T)}
    
    # 在/etc/profile unset i前增加PKG_CONFIG_PATH
    grep '^PKG_CONFIG_PATH' /etc/profile||sed -i '/^unset i/iexport PKG_CONFIG_PATH=/usr/lib/pkgconfig/:/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH' /etc/profile
    source /etc/profile
    
    [[ -f /etc/ld.so.conf.d/opencv.conf ]] && cp -p /etc/ld.so.conf.d/opencv.conf{,.$(date +%F)}
    echo "/usr/local/lib/" > /etc/ld.so.conf.d/opencv.conf
    ldconfig
    ```


### 安装`php-ppc_CoreImgClip`扩展

```bash
# 下载
 git clone https://github.com/george518/ppc_coreimgclip.git

# 进入php_ext
cd php_ext/

# 编译安装
phpize
./configure --with-php-config=/usr/bin/php-config
make
make install

# 拷贝到php扩展目录
cp modules/ppc_coreimgclip.so /path/to/extension/

# 修改php.ini 加入ppc_coreimgclip
php -m|grep ppc_coreimgclip.so||echo 'extension = ppc_coreimgclip.so' >> /etc/php.ini

# 重启php-fpm
service php-fpm restart
```

### 一、在php中使用格式

* 示例：

    ```php
    <?php

        #需要处理的图片绝对路径
        $sourceImg = "/Users/haodaquan/Sites/Test/ppc_coreimgclip/bags.jpg";
        #生成图片需要保存的路径和名称
        $destImg   = "/Users/haodaquan/Sites/Test/ppc_coreimgclip/newbags.jpg";
        #生成图片的宽度
        $destWidth = 300;
        #生成图片的高度
        $destHeight = 600;
        #图片的位置：
        #1-水平垂直居中（默认）
        #2-水平居中靠上
        #3-水平居中靠下
        #4-垂直居中靠左
        #5-垂直居中靠右
        #6-左上角
        #7-右上角
        #8-左下角
        #9-右下角
        $posion     = 1;
        #边距
        $margin = 10;
        #颜色差值，越小越精准
        $dValue = 10;
    
        $res = ppc_coreimgclip($sourceImg,$destImg,$destWidth,$destHeight,$posion,$margin,$dValue);
        var_dump($res);
        
        # 裁剪成功 $res 为 true
        # 裁剪失败 $res 为 false `
    ```

有问题联系我
----
qq:41352963


