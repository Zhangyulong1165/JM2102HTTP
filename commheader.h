#ifndef COMMHEADER_H
#define COMMHEADER_H
#include <QString>
#include <QQueue>
#include <QNetworkReply>
#include <iostream>
#include <windows.h>
using namespace std;
#define MAX_DEVICE     10//最大设备数
#pragma pack(1)
///  Added By ZhangYL 2023-01-07
///  重庆超限数据对接
typedef struct
{
QString jm_cph;//    车牌号
QString jm_zs;//    轴数
QString jm_zsm;//    轴数描述
QString jm_zlzz;//    车货总重
QString jm_cxl;//    超限量
QString jm_deptid;//    所属机构
QString jm_cxll;//    车辆类型
QString jm_vehicle_Length;//    实测车长
QString jm_vehicle_Width;//    实测车宽
QString jm_vehicle_Height;//    实测车高
QString jm_vehicle_Speed;//    车速
QString jm_clsyr;//    车辆所有人
QString jm_pry_no;//    违法行为编号
QString jm_by1zz;//    复称标记
QString jm_illegalpictureurl1;//    证据图片url1

QString jm_illegalpictureurl2;  //证据图片url2
QString jm_illegalpictureurl3;  //证据图片url3
QString jm_illegalpictureurl4;  //证据图片url4
QString jm_illegalvideo5;  //证据视频url
QString jm_illegaltime;  //检测（违法）时间
QString jm_illegalstation;  //检测（违法）站点
QString jm_stationno;  //检测（违法）站点编号
QString jm_owneraddr;  //车辆所有人地址
QString jm_ownertel;  //车辆所有人电话
QString jm_passno;  //通行许可证号
QString jm_passline;  //通行许可证线路
QString jm_passstarttime;  //通行许可证开始时间
QString jm_passendtime;  //通行许可证结束时间
QString jm_passweight;  //通行许可证车货重量
QString jm_passgoods;  //通行许可证货物名称
QString jm_schecknO;  //数据序号

QString jm_hpzl;//号牌种类
QString jm_roadid;//案发路段
QString jm_stake_km;//桩号-KM
QString jm_stake_m;//桩号-M
QString jm_hight_waydir;//方向
//返回数据
QString RESULT_CODE;//接口调用状态标识
QString RESULT_MSG;//错误信息，调用成功等信息

}Overlimitdata;//重庆超限数据对接

typedef struct
{
    QString checkNo;            //检测单号
    QString siteCode;           //站点编号
    QString siteName;           //站点名称
    int siteType;           //站点类型21：超限检测站31:非现场检测点;41:高速公路入口检测;42:高速公路出口检测; 51:重点源头企业
    int checkType;          //检测数据类型 站点类型为21时 检测类型必填 检测类型: 1：初检； 2：复检
    int lane;               //检测车道编号 关联检测车道标识。按行进方向从左到右排列，依次为 1、2、 3、4
    int vehicleType;        //"车辆类型", // Integer 0：默认；1-货车2-客车；3-轿车；4-面包车；5- 其他
    QString checkTime;          //检测时间 格式为：yyyy-MM-dd HH:mm:ss
    QString plateNum;           //识别出的号牌号码。如：豫A12345
    int plateNumColor;       //车辆颜色 0-蓝色；1-黄色；2-黑色；3-白色；4-渐变绿色；5- 黄绿双拼色；6-蓝白渐变色；9-未确定；11-绿色；12-红色
    int plateNumType;       //号牌类型 01-大型汽车号牌；02-小型汽车号牌
    int direction;          //行驶方向 0-上行；1-下行
    int speed;              //车速 单位：km/h
    int axleCnt;            //车辆轴数
    int axleType;           //轴组类型
    int sumWeight;          //车祸总质量 精确到个位，单位:千克
    int limitWeight;        //最大允许总质量 精确到个位，单位:千克
    int overWeight;         //超限量 精确到个位，单位:千克
    double overRate;        //精确到小数点后2位，单位：%
    int width;              //车宽 精确到个位，单位 毫米
    int limitWidth;         //限宽 精确到个位，单位 毫米
    int overWidth;          //超宽 精确到个位，单位 毫米
    double overWidthRate;      //超宽百分比 精确到小数点后2位，单位：%
    int height;             //车高 精确到个位，单位 毫米
    int limitHeight;        //限高 精确到个位，单位 毫米
    int overHeight;         //超高 精确到个位，单位 毫米
    double overHeightRate;  //超高百分比 精确到小数点后2位，单位：%
    int length;             //车长 精确到个位，单位 毫米
    int limitLength;        //限长 精确到个位，单位 毫米
    int overLength;         //超长 精确到个位，单位 毫米
    double overLengthRate;  //超长百分比 精确到小数点后2位，单位：%
    int reverse;
    QString frontPlatePicPath; //车牌小图路径
    QString frontPicPath;   //车牌前抓拍图片路径
    QString backPicPath;    //车牌后抓拍图片路径
    QString sidePicPath;    //车牌侧抓拍图片路径
    QString topPicPath;     //车牌顶抓拍图片路径
    QString reversePicPath;     //车牌顶抓拍图片路径
    QString videoPath;   //车牌检测小视频路径
    unsigned int timeT;
    int isMatchFinished;    //是否匹配完成标志
    QString snapTime;
}JmVehSqlInfo;
typedef struct
{
    QString checkNo;            //检测单号
    QString siteCode;           //站点编号
    QString siteName;           //站点名称
    int siteType;           //站点类型21：超限检测站31:非现场检测点;41:高速公路入口检测;42:高速公路出口检测; 51:重点源头企业
    int checkType;          //检测数据类型 站点类型为21时 检测类型必填 检测类型: 1：初检； 2：复检
    int lane;               //检测车道编号 关联检测车道标识。按行进方向从左到右排列，依次为 1、2、 3、4
    int vehicleType;        //"车辆类型", // Integer 0：默认；1-货车2-客车；3-轿车；4-面包车；5- 其他
    QString checkTime;          //检测时间 格式为：yyyy-MM-dd HH:mm:ss
    QString plateNum;           //识别出的号牌号码。如：豫A12345
    int plateNumColor;       //车辆颜色 0-蓝色；1-黄色；2-黑色；3-白色；4-渐变绿色；5- 黄绿双拼色；6-蓝白渐变色；9-未确定；11-绿色；12-红色
    int plateNumType;       //号牌类型 01-大型汽车号牌；02-小型汽车号牌
    int direction;          //行驶方向 0-上行；1-下行
    int speed;              //车速 单位：km/h
    int axleCnt;            //车辆轴数
    int axleType;           //轴组类型
    int sumWeight;          //车祸总质量 精确到个位，单位:千克
    int limitWeight;        //最大允许总质量 精确到个位，单位:千克
    int overWeight;         //超限量 精确到个位，单位:千克
    double overRate;        //精确到小数点后2位，单位：%
    int width;              //车宽 精确到个位，单位 毫米
    int limitWidth;         //限宽 精确到个位，单位 毫米
    int overWidth;          //超宽 精确到个位，单位 毫米
    double overWidthRate;      //超宽百分比 精确到小数点后2位，单位：%
    int height;             //车高 精确到个位，单位 毫米
    int limitHeight;        //限高 精确到个位，单位 毫米
    int overHeight;         //超高 精确到个位，单位 毫米
    double overHeightRate;  //超高百分比 精确到小数点后2位，单位：%
    int length;             //车长 精确到个位，单位 毫米
    int limitLength;        //限长 精确到个位，单位 毫米
    int overLength;         //超长 精确到个位，单位 毫米
    double overLengthRate;  //超长百分比 精确到小数点后2位，单位：%
    int reverse;            //"违法标志", //Integer。0正常，1逆行，2超速行驶，3机占非、4占用专用车道，5城市违停，6不安导向车道行驶，7路口停车，8路口滞留，9未系安全带，10违法禁令标志，11未礼让行人，12违反信号灯，13低速行驶，14违法停车，15不安规定行驶，16违法倒车，17压线，18违法变道。其他待定。
}WeightJsonInfo;
typedef struct
{
    QString checkNo;        //检测单号 需与过车检测数据检测单号保持一致
    QString siteCode;           //站点编号
    QString siteName;           //站点名称
    int siteType;           //站点类型 21：超限检测站31:非现场检测点;41:高速公路入口检测;42:高速公路出口检测; 51:重点源头企业
    int lane;
    int direction;
    QString snapTime;
    double overRate;        //超限超载率 精确到小数点后2位，单位：%
    int transType;          //照片传输方式 1字符串，参数：2 ；定义为上传图片、视频文件经过 BASE64 转码后的字符串； 参数：3：定义为上传的图片URL；
    int fileType;           //文件类型 Integer 11 车牌小图 12 车头照片 13 车尾照片 14 前侧照片 15 后侧照片 21 全景短视频  22车头短视频 23车尾短视频 24前侧短视频 25后侧短视频
    QString fileContent;    //文件string base64
    QString postfix;        //"文件后缀", string  如jpg  bmp  mp4 avi 其他格式再定
}PicJsonInfo;
typedef struct {
    string deviceId;	//检测设备ID
    string stationId;	//检测站点编号
    string matchNo;		//匹配编号
    int lane;			//车道号
    int sumWeight;	//总重,单位是千克
    int axleCnt;		//轴数
    int axleGrpCnt;		//轴组数
    string axleWeight;   //轴重 例如： 1轴重, 2轴重, 3轴重, 4轴重, 5轴重, 6轴重。多个轴用逗号分隔
    string lAxleWeight;	//单轴左轮重量 例如： 左1轴重,左2轴重,左3轴重,左4轴重,左5轴重,左6轴重。多个轴用逗号分隔
    string rAxleWeight;	//单轴右轮重量 例如： 右1轴重,右2轴重,右3轴重,右4轴重,右5轴重,右6轴重。多个轴用逗号分隔
    int axleType;	//轴型 1:单胎，2：双胎 例如： 轴型1,轴型2,轴型3,轴型4,轴型5,轴型6。多个轴型用逗号分隔
    string axleDis;		//轴距 例如： 轴距1, 轴距2, 轴距3, 轴距4, 轴距5, 轴距6。多个轴距用逗号分隔
    string axleGrpType; //轴组类型 1：单轴单轮； 2：单轴双轮； 3：双联轴单轮； 4：双联轴单双轮 5：双联轴双轮； 6：三联轴单轮； 7：三联轴双轮 例如： 轴组型1, 轴组型2, 轴组型3, 轴组型4, 轴组型5, 轴组型6。多个轴组型用逗号分隔
    int length;         //长 单位cm
    int speed;			//时速 公里/小时
    int runState;		//行驶状态
    int reverse;		//0 为正向；1 为逆向
    string attachmentNo;	//附属文件匹配编号
    string dateTime;	//检测时间 格式：yyyy - MM - dd HH : mm : ss.SSS
    long long timeT;
}RecvVecData;
typedef struct
{
    long long imgSize;
    string deviceId;
    int cameraDir;		//摄像头抓拍车辆位置： 0 未知 1 前抓 2后抓 3侧抓 4后抓 5逆向 6取证抓拍 21前车牌小图 22后车牌小图 23侧车牌小图 24顶车牌小图
    string stationId;	//检测站点id
    string matchNo;		//匹配编号
    string plateNum;	//车牌号
    int entireBelieve;	//车牌号置信度
    int plateType;		//0-0- 标准民用车与军车车牌 1- 02式民用车牌 2- 武警车车牌  3- 警车车牌 4- 民用车双行尾牌 5- 使馆车牌 6- 农用车车牌 7- 摩托车车牌 8- 新能源车牌
    int plateColor;		//0 - 蓝色车牌 1 - 黄色车牌 2 - 白色车牌  3 - 黑色车牌 4 - 绿色车牌 5 - 民航黑色车牌 0xff - 其他
    int colorDepth;		//车身颜色深浅，0-深色，1-浅色
    int color;			//车身颜色，0-其他色，1-白色，2-银色，3-灰色，4-黑色，5-红色，6-深蓝，7-蓝色，8-黄色，9-绿色，10-棕色，11-粉色，12-紫色，13-深灰，14-青色，0xff-未进行车身颜色识别
    int length;			//长 单位cm
    int width;			//宽 单位cm
    int height;			//高 单位cm
    int lane;			//车道号
    int vehicleType;	//"车辆类型", // Integer 0：默认；1-货车2-客车；3-轿车；4-面包车；5- 其他
    int speed;			//车速  公里/小时
    int direction;		//0-其它，1-从东向西 ，2-从西向东，3-从南向北 ，4-从北向南 ，5-从东南向西北，6-从西北向东南，7-从东北向西南，8-从西南向东北
    int vehicleLogoRecog;//车辆主品牌
    int illegalType;	//0-正常，1-低速，2-超速，3-逆行，4-闯红灯，5-压车道线，6-不按导向，7-路口滞留，8-机占非，9-违法变道，10-机动车违反规定使用专用车道，11-黄牌车禁限，12-路口停车，13-绿灯停车，14-未礼让行人，15-违章停车，16-违章掉头，17-占用应急车道，18-禁右，19-禁左，20-压黄线，21-未系安全带，22-行人闯红灯，23-加塞
    string dateTime;	//检测时间 格式：yyyy-MM-dd HH:mm:ss.SSS
    int picNum;			//图片数量
    string postfix;		//文件后缀
    string illegalCode;//违章代码
    int reverse;
    long long timeT;
    string snapTime;
}RecvCamData;
typedef struct
{
    string matchNo;
    string filePath;
    long long size;
    int direction;
    int cameraDir;//摄像头抓拍车辆位置： 0 未知 1 前抓 2后抓 3侧抓 4后抓 5逆向 6取证抓拍 21前车牌小图 22后车牌小图 23侧车牌小图 24顶车牌小图
    string postfix;		//文件后缀
    int isFinish;
    long long timeT;
}RecvPicData;
typedef struct
{
    QString checkNo;
    QNetworkReply *reply;
    int isFinish;
    int fileType;
    unsigned int timeT;
}ReplyData;
#pragma pack ()
#endif // COMMHEADER_H
