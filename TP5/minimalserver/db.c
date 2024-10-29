#include <stdio.h>
#include <stdlib.h>
#include "postgres/libpq-fe.h"

void make_read_request() {
   
}

int main(int argc, char *argv[])
{
   char *connection_info = "dbname=postgres user=postgres password=postgres host=172.28.100.142 port=5000";

   // Initiate connection
   PGconn *db_connection = PQconnectdb(connection_info);

   if (PQstatus(db_connection) != CONNECTION_OK)
   {
      printf("Error while connecting to the database server: %s\n", PQerrorMessage(db_connection));
      PQfinish(db_connection);
      return -1;
   }

   // We have successfully established a connection to the database server
   printf("Connection Established\n");
   printf("Port: %s\n", PQport(db_connection));
   printf("Host: %s\n", PQhost(db_connection));
   printf("DBName: %s\n", PQdb(db_connection));

   // Execute a query
   char *query = "SELECT * FROM test_table;";
   // char *query = "INSERT INTO test_table VALUES (1, 'nom')";

   // Submit the query and retrieve the result
   PGresult *response = PQexec(db_connection, query);

   // Check the status of the query result
   ExecStatusType response_status = PQresultStatus(response);

   // Convert the status to a string and print it
   printf("Query Status: %s\n", PQresStatus(response_status));

   // Check if the query execution was successful
   if (response_status != PGRES_TUPLES_OK && response_status != PGRES_COMMAND_OK)
   {
      printf("Error while executing the query: %s\n", PQerrorMessage(db_connection));
      PQclear(response);
      PQfinish(db_connection);
      return -1;
   }

   printf("Query Executed Successfully\n");

   int rows = PQntuples(response);
   int cols = PQnfields(response);
   printf("Number of rows: %d\n", rows);
   printf("Number of columns: %d\n", cols);

   // Print the column names
   for (int i = 0; i < cols; i++)
   {
      printf("%s\t", PQfname(response, i));
   }
   printf("\n");

   // Print all the rows and columns
   for (int i = 0; i < rows; i++)
   {
      for (int j = 0; j < cols; j++)
      {
         // Print the column value
         printf("%s\t", PQgetvalue(response, i, j));
      }
      printf("\n");
   }
   PQclear(response);
   PQfinish(db_connection);

   return 0;
}
