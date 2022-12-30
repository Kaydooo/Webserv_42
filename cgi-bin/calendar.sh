#!/bin/bash

echo -e 'HTTP/1.1 200 OK'
echo -e 'Content-Type: text/html\r\n\r\n'

echo '<h3>'
echo 'Calendar:'
echo '</h3>'


echo ''
echo '<pre>' 
cal 
echo '</pre>' 
echo ''

echo '<h3>'
echo 'Date: '
echo '</h3>'
echo '<pre>'
date
date -u
echo '</pre>'

echo '<h4>'
echo 'Days have passed since the beginning of the year: '
echo '</h4>'
echo '<pre>'
date +%j
echo '</pre>'
