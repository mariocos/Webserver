# Webserver

A HTTP/1.1 Web Server made in C++ 98.

## Features

- YAML configuration
- HTTP/1.1 protocol support
- Multiple Server binding support
- Custom error pages
- GET, POST and DELETE methods supported
- Static file serving
- Directory Indexing and Listing
- CGI scripts
- File uploads
- HTTP Redirects

## Usage

```bash
./webserv config_file_name
```

## Configuration

The server configuration is based on a YAML file.
This is an example of a configuration file:

```yaml
servers:
  - listen: 8080
    server_names: localhost
    max_connections: 3
    default: true
    error_pages:
      404: ./website/404.html
    routes:
      - uri: /
        methods: [GET]
        modules:
          - type: static
            settings:
              root: ~
              directory_listing: true
```

- `listen`: Port to listen on.
- `server_names`: Name that server will respond to.
- `max_connections`: Number of maximum connections allowed (if not defined is unlimited).
- `default`: If true, this server will be the default one (only one can be the default).
- `error_pages`: List of files to be served in the error code specified.
- `routes`: List of routes.
  - `uri`: URI that route will respond to.
  - `methods`: List of allowed methods (if empty no method is allowed).
  - `modules`: List of modules to be executed for this route.
    - `type`: Module type.
    - `settings`: Module settings.
   
More examples of configurations in the config_files folder.

## Authors

- [Mario Souza](https://github.com/mariocos)
- [Paulo Ribeiro](https://github.com/PBjr2002)
- [André Santos](https://github.com/DeblinPT)

## License

This project is part of the 42 school curriculum.

