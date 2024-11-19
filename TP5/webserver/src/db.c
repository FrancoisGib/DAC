#include "db.h"

char *postgres_url = NULL;
char *postgres_port = NULL;

void load_env(void)
{
   postgres_url = getenv("POSTGRES_URL");
   postgres_port = getenv("POSTGRES_PORT");
}

int detector_basic_sql_injection(char *source)
{
   char *regexString = "([a-zA-Z]*)(SELECT|UPDATE|ALTER|INSERT|select|update|alter|insert)([a-zA-Z]*)";
   size_t maxGroups = 3;

   regex_t regexCompiled;
   regmatch_t groupArray[maxGroups];

   if (regcomp(&regexCompiled, regexString, REG_EXTENDED))
   {
      printf("Could not compile regular expression.\n");
      return 1;
   };

   int match = 0;

   if (regexec(&regexCompiled, source, maxGroups, groupArray, 0) == 0)
   {
      unsigned int g = 0;
      for (g = 0; g < maxGroups; g++)
      {
         if (groupArray[g].rm_so == -1)
            break;

         char sourceCopy[strlen(source) + 1];
         strcpy(sourceCopy, source);
         sourceCopy[groupArray[g].rm_eo] = 0;
         printf("Group %u: [%2u-%2u]: %s\n",
                g, groupArray[g].rm_so, groupArray[g].rm_eo,
                sourceCopy + groupArray[g].rm_so);

         match++;
      }
   }

   regfree(&regexCompiled);
   return match > 0;
}

int make_read_request(char *username, char *password)
{
   if (strlen(username) > 25 || strlen(password) > 25 || detector_basic_sql_injection(username) || detector_basic_sql_injection(password))
   {
      return -1;
   }

   if (postgres_port == NULL || postgres_url == NULL)
   {
      load_env();
   }

   char connection_info[128];
   sprintf(connection_info, "dbname=postgres user=postgres password=postgres host=%s port=%s", postgres_url, postgres_port);
   printf("host %s, port %s\n", postgres_url, postgres_port);

   clock_t start, end;
   start = clock();
   PGconn *db_connection = PQconnectdb(connection_info);

   if (PQstatus(db_connection) != CONNECTION_OK)
   {
      printf("Error while connecting to the database server: %s\n", PQerrorMessage(db_connection));
      PQfinish(db_connection);
      return -1;
   }
   printf("Connection Established\n");
   printf("Port: %s\n", PQport(db_connection));
   printf("Host: %s\n", PQhost(db_connection));
   printf("DBName: %s\n", PQdb(db_connection));

   char query[128];
   sprintf(query, "SELECT * FROM test_table WHERE username = %s AND password = %s;", username, password);
   printf("Query: %s\n", query);
   // char *query = "INSERT INTO test_table VALUES (1, 'nom')";

   PGresult *response = PQexec(db_connection, query);
   ExecStatusType response_status = PQresultStatus(response);
   printf("Query Status: %s\n", PQresStatus(response_status));
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

   for (int i = 0; i < cols; i++)
   {
      printf("%s\t", PQfname(response, i));
   }
   printf("\n");

   for (int i = 0; i < rows; i++)
   {
      for (int j = 0; j < cols; j++)
      {
         printf("%s\t", PQgetvalue(response, i, j));
      }
      printf("\n");
   }
   PQclear(response);
   PQfinish(db_connection);
   end = clock();
   double request_time = ((double)(end - start)) / CLOCKS_PER_SEC;
   printf("Database request time: %lf\n", request_time);
   return rows > 0;
}
