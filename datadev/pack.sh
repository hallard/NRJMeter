rm js/libraries.min.js.gz

cat js/jquery-3.2.1.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/jquery.terminal-1.7.2.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/jquery.formautofill.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/tether.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/popper.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/bootstrap.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/bootstrap-toggle.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/bootstrap-slider.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/bootstrap-switch.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/bootstrap-table.mod.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/validator.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/reconnecting-websocket.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/Chart.bundle.min.js >> js/libraries.min.js
printf '\n' >> js/libraries.min.js
cat js/notify.min.js >> js/libraries.min.js
gzip -9 js/libraries.min.js

rm css/libraries.min.css.gz

cat css/bootstrap.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/bootstrap-grid.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/bootstrap-reboot.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/bootstrap-toggle.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/bootstrap-slider.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/bootstrap-switch.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/bootstrap-table.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/tether.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/tether-theme-basic.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/jquery.terminal-1.7.2.min.css >> css/libraries.min.css
printf '\n' >> css/libraries.min.css
cat css/font-awesome.min.css >> css/libraries.min.css
gzip -9 css/libraries.min.css

mv js/libraries.min.js.gz ../data/js
mv css/libraries.min.css.gz ../data/css
