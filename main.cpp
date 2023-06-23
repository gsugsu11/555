#include <QCoreApplication>
#include <math.h>
#include <QDebug>



#include <QFile>
#include <QTextStream>
#include <QPointF>

class MyClass {
public:
    QString stringVal;
    QPointF pointVal;
    QVector<QPointF> pointListVal{{0.0,0.0}};//这里初始化要赋值个值,不然存储没赋值的类,会少一项,从而出错
    bool isEmpty(){//防止从文件读取到无效数据,这里还可增加判断用或判断
        if( stringVal.isEmpty() ){
            return true;
        }else{
            return false;
        }
    }
    // 实现读操作符，从CSV文件中读取数据并存储到MyClass对象中
    friend QTextStream &operator >>(QTextStream &in, MyClass &myClass) {
        QString line = in.readLine(); // 读取一行数据
        if( line.isEmpty() ){
            return in;
        }
        QStringList fields = line.split(','); // 以","为分隔符，分离出每个字段的值
        if( fields.size() != 3 ){
            return in;
        }
        QString stringValStr = fields.at(0); // 取出第一个字段（QString类型）
        myClass.stringVal = stringValStr; // 存储到MyClass对象的stringVal变量中
        QStringList xyList = fields.at(1).split( ':' );
        if( xyList.size() != 2 ){
            return in;
        }
        QString xValStr = fields.at(1).split( ':' ).at(0); // 取出第二个字段（pointVal的x值）
        QString yValStr = fields.at(1).split( ':' ).at(1); // 取出第二个字段（pointVal的y值）
        qreal xVal = xValStr.toDouble(); // 转换成浮点数
        qreal yVal = yValStr.toDouble();
        QPointF pointVal(xVal, yVal); // 创建 QPointF 对象
        myClass.pointVal = pointVal; // 存储到MyClass对象的pointVal变量中
        QString pointsListStr = fields.at(2); // 取出第三个字段
        QStringList pointStrList = pointsListStr.split(';'); // 以";"为分隔符，分离出每个 QPointF 的字符串表示
        QVector<QPointF> pointListVal; // 创建 QVector<QPointF> 对象，存储 pointStrList 中每个 QPointF 对象
        for(int i = 0; i < pointStrList.size(); i++) {
            QStringList pointStrings = pointStrList.at(i).split(':'); // 以","为分隔符，分离出每个QPointF对象的x和y值
            if( pointStrings.size() != 2  ){
                continue;
            }
            qreal x = pointStrings.at(0).toDouble();
            qreal y = pointStrings.at(1).toDouble();
            QPointF point(x, y);
            pointListVal.append(point); // 将分离出的x,y组合成点，并添加到QVector中
        }
        myClass.pointListVal = pointListVal; // 存储到MyClass对象的pointListVal变量中

        return in;
    }

    // 实现写操作符，将MyClass对象的值写入到CSV文件中
    friend QTextStream &operator <<(QTextStream &out, const MyClass &myClass) {
        out << myClass.stringVal // 写入第一个字段
            << "," << myClass.pointVal.x() // 写入pointVal的x值
            << ":" << myClass.pointVal.y() // 写入pointVal的y值
            << ",";
        QVector<QPointF> pointListVal = myClass.pointListVal; // 获取pointListVal的引用
        for(int i = 0; i < pointListVal.size(); i++) {
            QPointF point = pointListVal.at(i); // 取出每个QPointF对象
            out << QString::number(point.x()) // 写入x值
                << ":" << QString::number(point.y()); // 写入y值
            if(i != pointListVal.size() - 1) { // 判断是否为最后一个QPointF
                out << ";"; // 如果不是，写入";"
            }
        }
        return out;
    }
};

// 用于从CSV文件中读取数据至QVector<MyClass>的函数
template < class T >
void readFromCsv( QString filePath, QVector< T > &myClassList ) {
    QFile file(filePath); // 创建文件对象
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { // 打开文件，如果出错，退出函数
        return;
    }

    QTextStream in(&file); // 创建文件流
    while(!in.atEnd()) { // 逐行读取数据
        T myClass; // 创建 MyCLass 对象
        in >> myClass; // 用读操作符读取一行数据
        if( myClass.isEmpty() ){
            continue;
        }
        myClassList.append(myClass); // 将 MyCLass 对象添加到 QVector 中
    }
    file.close(); // 关闭文件
    return;
}

// 用于将QVector<MyClass>数据写入CSV文件中的函数
template < class T >
void writeToCsv(QString filePath, QVector< T > &myClassList) {
    QFile file(filePath); // 创建文件对象
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { // 打开文件，如果出错，退出函数
        return;
    }

    QTextStream out(&file); // 创建文件流
    for(int i = 0; i < myClassList.size(); i++) { // 逐个 MyCLass 对象写入文件
        T myClass = myClassList.at(i);
        out << myClass << "\n"<< "\n"; // 用写操作符写入一行数据
    }
    file.close(); // 关闭文件
}


int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    MyClass mc1;
    mc1.stringVal = "stringVal";
    mc1.pointVal = QPointF( 1.0, 1.0 );
    mc1.pointListVal.append( QPointF( 2.0, 1.0 ) );
    mc1.pointListVal.append( QPointF( 3.0, 1.0 ) );
    mc1.pointListVal.append( QPointF( 4.0, 1.0 ) );

    MyClass mc2;
    mc2.stringVal = "stringVa2";
    mc2.pointVal = QPointF( 1.0, 2.0 );
    mc2.pointListVal.append( QPointF( 2.0, 2.0 ) );
    mc2.pointListVal.append( QPointF( 3.0, 2.0 ) );
    mc2.pointListVal.append( QPointF( 4.0, 2.0 ) );
    QVector<MyClass> myClassList;
    myClassList.append( mc1 );
    myClassList.append( mc2 );
    //writeToCsv( "myClass.csv", myClassList );
    qDebug() << "begin myClass.csv";
    QVector<MyClass> readMyClassV;
    readFromCsv( "myClass.csv", readMyClassV );
    qDebug() << "readMyClassV.size():" << readMyClassV.size();
    foreach(auto it, readMyClassV ){
        qDebug() << it.stringVal;
        qDebug() << it.pointVal.x() << ":" << it.pointVal.y();
        foreach( auto itp, it.pointListVal ){
             qDebug() << itp.x() << ":" << itp.y();
        }
    }


    return a.exec();
}
