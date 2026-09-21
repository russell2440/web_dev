FROM php:8.2-apache

# Install MySQL extensions for PHP
RUN docker-php-ext-install mysqli pdo pdo_mysql

# Tell Apache to serve from your 'deployed' folder
COPY deployed/ /var/www/html/

RUN a2enmod rewrite
EXPOSE 80
