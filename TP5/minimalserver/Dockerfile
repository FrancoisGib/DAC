# Start from a empty image
FROM scratch

# Copy executable
COPY http /

# Documentation
EXPOSE 8080
LABEL AUTHOR="François Gibier"

# Run the application
CMD ["/http", "8080"]