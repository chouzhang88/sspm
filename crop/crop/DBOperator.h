#include <Windows.h>
#include "mysql.h" 
#include "string" 
using namespace std;
class DBOperator
{
	MYSQL con;
	char dbuser[30] = "root";
	char dbpasswd[30] = "123456";
	char dbip[30] = "127.0.0.1";
	char dbname[50] = "crop";
	char tablename[50] = "t_crop_2008";
	char *query = NULL;
public:
	DBOperator();
	~DBOperator();
	void doConnect();
	void doQuery(string sql);
};

//�����������ֲ�����
//create table t_crop_2008 (f_id varchar(20), f_x integer(10),f_y integer(10),f_value integer(3), f_r float(8), f_g float(8), f_b float(8));

//���������õı��ṹ��R����arulesSequences����zaki����һ��
//create table t_out (f_sequence_id varchar(20),f_year integer(4),f_size integer(2),f_value integer(3));

//�Ӹ���ݵ���ֲ���ȡ���ݽ��뵼����
//insert into t_out(f_sequence_id,f_value) select f_id,f_value from t_crop_2010;

//ÿ��һ������ݣ��Ͱѵ������������ݸ���
//update t_out set f_year = 2010 where f_year is NULL;

//��mysql���ݿ�ı��а����ݵ����������ļ�
// select * into outfile "C:/ProgramData/MySQL/MySQL Server 5.7/Uploads/crop.txt" from t_out order by f_sequence_id asc,f_year asc;