#include "stdafx.h"
#include "DBOperator.h"


DBOperator::DBOperator()
{
}


DBOperator::~DBOperator()
{
}

void DBOperator::doConnect()
{
	
	mysql_init(&con);
	mysql_real_connect(&con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0);
	 
}

void DBOperator::doQuery(string sql)
{
	if (&con != NULL)
	{
		
		int rt = mysql_real_query(&con, sql.c_str(), sql.length());
		if (rt)
		{
			printf("Error making query: %s !!!\n", mysql_error(&con));
		}else
		{
			//printf("%s executed!!!\n", sql.c_str());
		 }
	}
}
