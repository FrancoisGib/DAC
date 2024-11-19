#include "ssl.h"

void initialize_ssl(void)
{
   SSL_load_error_strings();
   SSL_library_init();
   OpenSSL_add_all_algorithms();
}

SSL_CTX *create_ssl_context(void)
{
   const SSL_METHOD *method = TLS_server_method();
   if (!method)
   {
      perror("Unable to get SSL method");
      ERR_print_errors_fp(stderr);
      exit(EXIT_FAILURE);
   }

   SSL_CTX *ctx = SSL_CTX_new(method);
   if (!ctx)
   {
      perror("Unable to create SSL context");
      ERR_print_errors_fp(stderr);
      exit(EXIT_FAILURE);
   }

   if (SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION) != 1)
   {
      perror("Failed to set minimum TLS version to 1.3");
      ERR_print_errors_fp(stderr);
      SSL_CTX_free(ctx);
      exit(EXIT_FAILURE);
   }

   if (SSL_CTX_use_certificate_file(ctx, "cert/cert.pem", SSL_FILETYPE_PEM) <= 0)
   {
      perror("Failed to load certificate");
      ERR_print_errors_fp(stderr);
      SSL_CTX_free(ctx);
      exit(EXIT_FAILURE);
   }

   if (SSL_CTX_use_PrivateKey_file(ctx, "cert/key.pem", SSL_FILETYPE_PEM) <= 0)
   {
      perror("Failed to load private key");
      ERR_print_errors_fp(stderr);
      SSL_CTX_free(ctx);
      exit(EXIT_FAILURE);
   }

   if (SSL_CTX_check_private_key(ctx) != 1)
   {
      fprintf(stderr, "Private key does not match the certificate\n");
      ERR_print_errors_fp(stderr);
      SSL_CTX_free(ctx);
      exit(EXIT_FAILURE);
   }

   return ctx;
}
