var ws;
var f;
var el_timer;
var counters = {};
var elapsed = 0;
var isousc, iinst;
var ctx_mcp3421;
var ctx_si;
var ctx_sh;
var tinfo_chart;
var si7021_chart;
var sht10_chart;
var mcp3421_chart;
var has_si7021;
var has_sht10;
var has_mcp3421;
var is_local = false;
var socksrv;
var term;
var logs = false; //by default
var urls = {
    tinfo: 'tinfo',
    sensors: 'sensors',
    hb: "hb",
    reset: "reset",
    config_reset: "config_reset",
    factory_reset: "factory_reset",
    config_form: "config_form",
    update: "update",
    spiffs: "spiffs",
    system: "system",
    config: "config",
    version: "version.json",
    wifiscan: "wifiscan"
};

function isJson(item) {
    if ((item.charAt(0) == '{' && item.slice(-1) == '}') || (item.charAt(0) == '[' && item.slice(-1) == ']'))
        return true;
    else
        return false;
}

function ts() {
    var myDate = new Date();
    var ts = '';
    if (myDate.getHours() < 10) ts += '0';
    ts += myDate.getHours() + ':';
    if (myDate.getMinutes() < 10) ts += '0';
    ts += myDate.getMinutes() + ':';
    if (myDate.getSeconds() < 10) ts += '0';
    ts += myDate.getSeconds() + ' ';
    return ts;
}

function getActiveTab() {
    return ($('.nav-tabs .active > a').attr('href'));
}

function formatSize(bytes) {
    if (bytes < 1024) return bytes + ' Bytes';
    if (bytes < (1024 * 1024)) return (bytes / 1024).toFixed(0) + ' KB';
    if (bytes < (1024 * 1024 * 1024)) return (bytes / 1024 / 1024).toFixed(0) + ' MB';
    return (bytes / 1024 / 1024 / 1024).toFixed(0) + ' GB';
}

function rowStyle(row, index) {
    //var classes=['active','success','info','warning','danger'];  
    var flags = parseInt(row.fl, 10);
    //if (flags & 0x80) return {classes:classes[4]};
    //if (flags & 0x02) return {classes:classes[3]};
    //if (flags & 0x08) return {classes:classes[1]};
    if (flags & 0x80) return {
        classes: 'danger'
    };
    if (flags & 0x02) return {
        classes: 'warning'
    };
    if (flags & 0x08) return {
        classes: 'success'
    };
    return {};
}

function labelFormatter(value, row) {
    var flags = parseInt(row.fl, 10);

    if (typeof counters[value] === 'undefined')
        counters[value] = 1;
    if (flags & 0x88)
        counters[value]++;
    return value + ' <span class=\"badge badge-pill badge-secondary\">' + counters[value] + '</span>';
}

function deleteButtonMaker(value, row) {
    var htm = '';

    htm += '<button type="button" class="btn btn-danger btn-sm" data-toggle="modal" data-target="#confirm-del" data-file2delete="' + row.na + '" >';
    htm += 'Delete';
    htm += '</button>';

    return htm;
}

function fileFormatter(value, row) {
    var fname = row.na;
    var htm = '';
    //ht+= '<button type="button" class="btn btn-xs btn-danger" title="Supprimer">';
    //ht+= '<span class="glyphicon glyphicon-trash" aria-hidden="true"></span>&nbsp;';
    //ht+= '</button>&nbsp;';
    //ht+= '<button type="button" class="btn btn-xs btn-primary" title="Télécharger">';
    //ht+= '<span class="glyphicon glyphicon-trash" aria-hidden="true"></span>&nbsp;';
    //ht+= '</button>&nbsp;';
    htm += '<a href="' + fname + '">' + fname + '</a>';

    return htm;
}

function SSIDFormatter(value, row) {
    var ssid = value;
    var enc = String(row.enc);
    var opn = (enc == 'Open') ? true : false;

    var cl, htm;

    htm = "<div>";

    cl = 'success';
    if (opn)
        cl = 'eye';
    else
        cl = 'lock';

    //if (enc=='WPAOpen') cl='eye-open';  

    htm += "<span class='fa fa-" + cl + "'></span>&nbsp;" + ssid;
    if (!opn)
        htm += "&nbsp;<span class='badge badge-secondary label-" + cl + "'> " + enc + " </span> ";
    htm += "</div>";

    return htm;
}

function RSSIFormatter(value, row) {
    var rssi = parseInt(row.rssi);
    var signal = Math.min(Math.max(2 * (rssi + 100), 0), 100);
    var cl, htm;

    cl = 'success';
    if (signal < 70) cl = 'info';
    if (signal < 50) cl = 'warning';
    if (signal < 30) cl = 'danger';
    cl = 'bg-' + cl;

    htm = "<div class='progress progress-tbl'>";
    htm += "<div class='progress-bar " + cl + "' role='progressbar' aria-valuemin='0' aria-valuemax='100' ";
    htm += "aria-valuenow='" + signal + "' style='width:" + signal + "%'><span>" + signal + "%</span></div></div>";

    return htm;
}

function sliderFormatter(itype, value, type, unit) {
    if (value.length == 2) {
        var txt;
        var res;
        var item = '#' + itype + '_';

        item += unit == '%' ? 'hum' : 'temp';
        item += '_' + type + '_help';
        item = $(item);

        //console.log('item='+item);

        if (type == 'led') {
            txt = 'Green from ' + value[0] + unit + ' to ' + value[1] + unit;
            $(item).text('Select analog output range for temperature from 0V to 10V. Current ' + txt);
            return txt;
        }
    }
}

function valueFormatter(value, row) {
    if (row.na == "ISOUSC")
        isousc = parseInt(value);
    else if (row.na == "IINST") {
        var pb, pe, cl;
        iinst = parseInt(value);
        pe = parseInt(iinst * 100 / isousc);
        if (isNaN(pe))
            pe = 0;
        cl = 'success';
        if (pe > 70) cl = 'info';
        if (pe > 80) cl = 'warning';
        if (pe > 90) cl = 'danger';

        cl = 'bg-' + cl;
        if (pe > 0)
            $('#tcharge').text(pe + '%');
        if (typeof isousc != 'undefined')
            $('#pcharge').text(iinst + 'A / ' + isousc + 'A');
        $('#pbar').attr('class', 'progress-bar ' + cl);
        $('#pbar').css('width', pe + '%');
    }
    return value;
}

$.notify.addStyle('notifyhtml', {
    html: "<div data-notify-html='html'/>"
});

function Notify(mydelay, myicon, mytype, mytitle, mymsg) {
    $.notify({
        html: "<div class='card card-" + mytype + "'><strong><span class='fa fa-" + myicon + "'/>&nbsp;" + mytitle + "</strong><p>" + mymsg + "</p></div>"
    }, {
        autoHideDelay: 1000 * mydelay,
        style: 'notifyhtml'
    });
}

function progressUpload(data) {
    if (data.lengthComputable) {
        var pe = (data.loaded / data.total * 100).toFixed(0);
        $('#pfw').css('width', pe + '%');
        $('#psfw').text(formatSize(data.loaded) + ' / ' + formatSize(data.total));
    }
}

function waitReboot() {
    var url = location.href;
    $('#txt_srv').text('Trying to connect to ' + url);
    $('#mdl_wait').modal();
    el_timer = setInterval(function () {
        elapsed++;
        $('#txt_elapsed').text('Time elapsed ' + elapsed + ' s');
    }, 1000);
}

function refreshTinfo(tinfo_data) {
    //console.log('refreshTinfo=' + data);
    var tinfo_papp_iinst;
    var lbltext = "Not Found/Enabled";
    var now = new Date();
    var hour = now.getHours();
    var min = now.getMinutes();
    var sec = now.getSeconds();

    var label = hour < 10 ? '0' + hour : hour;
    label += ':';
    label += min < 10 ? '0' + min : min;
    label += ':';
    label += sec < 10 ? '0' + sec : sec;

    if (typeof (tinfo_data.papp_iinst) != 'undefined')
        tinfo_papp_iinst = tinfo_data.papp_iinst[0];

    if (typeof (tinfo_papp_iinst) != 'undefined' && tinfo_papp_iinst.seen >= 0) {
        $("#sp_tinfo_papp_iinst_state").text("");
        $("#col_tinfo_papp_iinst").removeClass('in').addClass('show');
        $("#sp_tinfo_papp").text(parseInt(tinfo_papp_iinst.PAPP) + 'W');
        $("#sp_tinfo_iinst").text(parseInt(tinfo_papp_iinst.IINST) + 'A');

        if (tinfo_chart.data.datasets[0].data.length >= 20) {
            tinfo_chart.data.labels.shift();
            tinfo_chart.data.datasets[0].data.shift();
            tinfo_chart.data.datasets[1].data.shift();
        }

        tinfo_chart.data.labels.push(label);
        tinfo_chart.data.datasets[0].data.push(parseInt(tinfo_papp_iinst.PAPP));
        tinfo_chart.data.datasets[1].data.push(parseInt(tinfo_papp_iinst.IINST));
        tinfo_chart.update();
    } else {
        if ($("#sp_tinfo_papp_iinst_state").text() != lbltext) {
            $("#sp_tinfo_papp_iinst_state").text(lbltext);
            $("#panhead_tinfo_papp_iinst").trigger("click");
        }
    }

    if (typeof (tinfo_data.tinfo) != 'undefined')
        $('#tab_tinfo_data').bootstrapTable('load', tinfo_data.tinfo);

}

function refreshSensors(sensors_data) {
    //console.log('refreshSensors=' + data);
    var si7021;
    var sht10;
    var mcp3421;
    var lbltext = "Not Found/Enabled";
    var now = new Date();
    var hour = now.getHours();
    var min = now.getMinutes();
    var sec = now.getSeconds();

    var label = hour < 10 ? '0' + hour : hour;
    label += ':';
    label += min < 10 ? '0' + min : min;
    label += ':';
    label += sec < 10 ? '0' + sec : sec;

    if (typeof (sensors_data.si7021) != 'undefined')
        si7021 = sensors_data.si7021[0];
    if (typeof (sensors_data.sht10) != 'undefined')
        sht10 = sensors_data.sht10[0];
    if (typeof (sensors_data.mcp3421) != 'undefined')
        mcp3421 = sensors_data.mcp3421[0];

    if (typeof (mcp3421) != 'undefined' && mcp3421.seen >= 0) {
        $("#sp_mcp3421_state").text('');

        if ($("#col_mcp3421").hasClass('in') && !$("#col_si7021").hasClass('show') && !$("#col_sht10").hasClass('show')) {
            $("#col_mcp3421").removeClass('in').addClass('show');
        }

        $("#sp_mcp3421_pwr").text(mcp3421.power + ' W');

        if (mcp3421_chart.data.datasets[0].data.length >= 20) {
            mcp3421_chart.data.labels.shift();
            mcp3421_chart.data.datasets[0].data.shift();
        }

        mcp3421_chart.data.labels.push(label);
        mcp3421_chart.data.datasets[0].data.push(mcp3421.power);
        mcp3421_chart.update();
        //mcp3421_gauge.set(Number(mcp3421.power));
    } else {
        if ($("#sp_mcp3421_state").text() != lbltext) {
            $("#sp_mcp3421_state").text(lbltext);
            $("#panhead_mcp3421").trigger("click");
        }
    }


    if (typeof (si7021) != 'undefined' && si7021.seen >= 0) {
        $("#sp_si7021_state").text('');

        if (!$("#col_mcp3421").hasClass('show') && $("#col_si7021").hasClass('in') && !$("#col_sht10").hasClass('show')) {
            $("#col_si7021").removeClass('in').addClass('show');
        }

        $("#sp_si7021_temp").text(parseFloat(si7021.temperature).toFixed(2) + '°C');
        $("#sp_si7021_hum").text(parseFloat(si7021.humidity).toFixed(2) + '%');

        if (si7021_chart.data.datasets[0].data.length >= 20) {
            si7021_chart.data.labels.shift();
            si7021_chart.data.datasets[0].data.shift();
            si7021_chart.data.datasets[1].data.shift();
        }

        si7021_chart.data.labels.push(label);
        si7021_chart.data.datasets[0].data.push(si7021.temperature);
        si7021_chart.data.datasets[1].data.push(si7021.humidity);
        si7021_chart.update();
    } else {
        if ($("#sp_si7021_state").text() != lbltext) {
            $("#sp_si7021_state").text(lbltext);
            $("#panhead_si7021").trigger("click");
        }
    }

    if (typeof (sht10) != 'undefined' && sht10.seen >= 0) {
        $("#sp_sht10_state").text('');

        if (!$("#col_mcp3421").hasClass('show') && !$("#col_si7021").hasClass('show') && $("#col_sht10").hasClass('in')) {
            $("#col_sht10").removeClass('in').addClass('show');
        }

        $("#sp_sht10_temp").text(parseFloat(sht10.temperature).toFixed(2) + '°C');
        $("#sp_sht10_hum").text(parseFloat(sht10.humidity).toFixed(2) + '%');

        if (sht10_chart.data.datasets[0].data.length >= 20) {
            sht10_chart.data.labels.shift();
            sht10_chart.data.datasets[0].data.shift();
            sht10_chart.data.datasets[1].data.shift();
        }

        sht10_chart.data.labels.push(label);
        sht10_chart.data.datasets[0].data.push(si7021.temperature);
        sht10_chart.data.datasets[1].data.push(si7021.humidity);
        sht10_chart.update();
    } else {
        if ($("#sp_sht10_state").text() != lbltext) {
            $("#sp_sht10_state").text(lbltext);
            $("#panhead_sht10").trigger("click");
        }
    }
}

$('#pan_edit_mM').click(function () {
    var card = $('#pan_edit');
    var span = $(this);

    if (!card.hasClass('fullscreen')) { //Going to maximize
        span.removeClass('fa-window-maximize').addClass('fa-window-minimize');
        card.addClass('fullscreen');
    } else { //Going to minimize
        span.removeClass('fa-window-minimize').addClass('fa-window-maximize');
        card.removeClass('fullscreen');
    }

});

function refreshSystem(system_data) {
    console.log('refreshSystem=' + system_data);
    $('#tab_sys_data').bootstrapTable('load', system_data);
}

function refreshLogger(logger_data) {
    console.log('refreshLogger=' + logger_data);
    $('#tab_log_data').bootstrapTable('load', logger_data);
    $('#tab_log_data').on('refresh.bs.table', function (e) {
        wsSend('$logger')
    });
}

function wsSend(message) {
    if (ws.readyState === 1)
        ws.send(message);
}

function toHex(val, n) {
    var txt = "";
    var hex = (+val).toString(16).toUpperCase();
    for (i = hex.length; i < n; i++)
        txt += "0";
    return (txt + hex);
}

function scanWifi() {

    var $table = $('#tab_scan_data');

    $table.bootstrapTable('showLoading');
    $.getJSON(urls['wifiscan'], function (scan_data) {

            //console.log("typeof(scan_data.status)="+typeof(scan_data.status));
            //if (typeof(scan_data.status)=='number') {
            //	var status = scan_data.status;
            //	console.log("scanWifi() status="+status);
            //}

            $table.bootstrapTable('hideLoading');
            if (typeof (scan_data.networks) == 'object') {
                $table.bootstrapTable('load', scan_data.networks, {
                    silent: true,
                    showLoading: false
                });
            }
        })
        .fail(function () {
            $table.bootstrapTable('hideLoading');
            console.log("Error while requesting Wifi networks");
        })
}

function refreshSpiffs() {
    wsSend('$spiffs');
    $.getJSON(urls['spiffs'], function (spiffs_data) {
            console.log("trying to parse spiffs_data..")
            var pb, pe, cl;
            total = spiffs_data.spiffs[0].Total;
            used = spiffs_data.spiffs[0].Used;
            freeram = spiffs_data.spiffs[0].Ram;

            $('#tab_fs_data').bootstrapTable('load', spiffs_data.files, {
                silent: true,
                showLoading: true
            });

            pe = parseInt(used * 100 / total);
            if (isNaN(pe))
                pe = 0;
            cl = 'success';
            if (pe > 70) cl = 'warning';
            if (pe > 85) cl = 'danger';

            cl = 'bg-' + cl;
            if (pe > 0)
                $('#sspiffs').text(pe + '%');
            $('#fs_use').text(formatSize(used) + ' / ' + formatSize(total));
            $('#pfsbar').attr('class', 'progress-bar ' + cl);
            $('#pfsbar').css('width', pe + '%');

        })
        .fail(function () {
            console.log("Error while requestiong SPIFFS data");
        })

    $.getJSON(urls['version'], function (spiffs_version) {
        var v = "SPIFFS v" + spiffs_version.version + '.' + spiffs_version.build;
        console.log('spiffs version=' + v);
        $("#spiffs_version").html(v);
    })
}

function fillConfigForm(data) {

    var tinfo_edf = data["tinfo_edf"] == 1 ? true : false;
    var tinfo_dsm = data["tinfo_dsm"] == 1 ? true : false;
    var sens_si7021 = data["sens_si7021"] == 1 ? true : false;
    var sens_sht10 = data["sens_sht10"] == 1 ? true : false;
    var sens_mcp3421 = data["sens_mcp3421"] == 1 ? true : false;

    var has_tinfo_edf = data["has_tinfo_edf"] == 1 ? true : false;
    var has_tinfo_dsm = data["has_tinfo_dsm"] == 1 ? true : false;
    var has_si7021 = data["has_si7021"] == 1 ? true : false;
    var has_sht10 = data["has_sht10"] == 1 ? true : false;
    var has_mcp3421 = data["has_mcp3421"] == 1 ? true : false;
    var has_oled = data["has_oled"] == 1 ? true : false;

    var cfg_ap = data["cfg_ap"] == 1 ? true : false;
    var cfg_wifi = data["cfg_wifi"] == 1 ? true : false;
    var cfg_mqtt = data["cfg_mqtt"] == 1 ? true : false;
    var cfg_debug = data["cfg_debug"] == 1 ? true : false;
    var cfg_logger = data["cfg_logger"] == 1 ? true : false;
    var cfg_tinfo = data["cfg_tinfo"] == 1 ? true : false;
    var cfg_demo = data["cfg_demo"] == 1 ? true : false;
    var cfg_rgb = data["cfg_rgb"] == 1 ? true : false;
    var cfg_oled = data["cfg_oled"] == 1 ? true : false;
    var cfg_static = data["cfg_static"] == 1 ? true : false;

    //var tinfo_pwr_led = JSON.parse("[" + data["tinfo_pwr_led"] + "]");
    //var tinfo_nrj_led = JSON.parse("[" + data["tinfo_nrj_led"] + "]");
    var sens_temp_led = JSON.parse("[" + data["sens_temp_led"] + "]");
    var sens_hum_led = JSON.parse("[" + data["sens_hum_led"] + "]");
    var sens_pwr_led = JSON.parse("[" + data["sens_pwr_led"] + "]");
    var led_panel = data["led_panel"];


    var mqtt_ret = data["mqtt_ret"] == 1 ? true : false;

    var cfg_led_bright = data["cfg_led_bright"];
    var cfg_led_hb = data["cfg_led_hb"];

    var emon_http = data["emon_http"] == 1 ? true : false;
    var emon_mqtt = data["emon_mqtt"] == 1 ? true : false;

    var jdom_http = data["jdom_http"] == 1 ? true : false;
    var jdom_mqtt = data["jdom_mqtt"] == 1 ? true : false;

    var domz_http = data["domz_http"] == 1 ? true : false;
    var domz_mqtt = data["domz_mqtt"] == 1 ? true : false;

    $("#frm_config").autofill(data);

    $("#cfg_mqtt").bootstrapSwitch({
        state: cfg_mqtt,
        onColor: "success"
    });

    $("#mqtt_ret").bootstrapSwitch({
        state: mqtt_ret,
        onColor: "success"
    });

    $("#emon_http").bootstrapSwitch({
        state: emon_http,
        onColor: "success"
    });

    $("#emon_mqtt").bootstrapSwitch({
        state: emon_mqtt,
        onColor: "success"
    });

    $("#jdom_http").bootstrapSwitch({
        state: jdom_http,
        onColor: "success"
    });

    $("#jdom_mqtt").bootstrapSwitch({
        state: jdom_mqtt,
        onColor: "success"
    });

    $("#domz_http").bootstrapSwitch({
        state: domz_http,
        onColor: "success"
    });

    $("#domz_mqtt").bootstrapSwitch({
        state: domz_mqtt,
        onColor: "success"
    });

    $("#tinfo_edf").bootstrapSwitch({
        state: tinfo_edf,
        onColor: has_tinfo_edf ? "success" : "warning",
        offColor: has_tinfo_edf ? "danger" : "warning"
    });

    $("#tinfo_dsm").bootstrapSwitch({
        state: tinfo_dsm,
        onColor: has_tinfo_dsm ? "success" : "warning",
        offColor: has_tinfo_dsm ? "danger" : "warning"
    });
    /*
    $("#tinfo_pwr_led").slider({
        id: "tinfo_pwr_led",
        value: tinfo_pwr_led,
        formatter: function (v) {
            return sliderFormatter('tinfo', v, 'led', 'W');
        }
    });
    $("#tinfo_nrj_led").slider({
        id: "tinfo_nrj_led",
        value: tinfo_nrj_led,
        formatter: function (v) {
            return sliderFormatter('tinfo', v, 'led', 'A');
        }
    });
    */
    $("#sens_temp_led").slider({
        id: "slider_temp_led",
        value: sens_temp_led,
        formatter: function (v) {
            return sliderFormatter('sens', v, 'led', '°C');
        }
    });
    $("#sens_hum_led").slider({
        id: "slider_hum_led",
        value: sens_hum_led,
        formatter: function (v) {
            return sliderFormatter('sens', v, 'led', '%');
        }
    });
    $("#sens_pwr_led").slider({
        id: "slider_pwr_led",
        value: sens_pwr_led,
        formatter: function (v) {
            return sliderFormatter('sens', v, 'led', 'W');
        }
    });

    $("#cfg_led_bright").slider({
        id: "slider_led_bright",
        value: cfg_led_bright,
        formatter: function (v) {
            return v + '%';
        }
    });
    $("#cfg_led_hb").slider({
        id: "slider_led_hb",
        value: cfg_led_hb,
        formatter: function (v) {
            return (v / 10) + 's';
        }
    });

    $("#sens_si7021").bootstrapSwitch({
        state: sens_si7021,
        onColor: has_si7021 ? "success" : "warning",
        offColor: has_si7021 ? "danger" : "warning"
    });
    $("#sens_sht10").bootstrapSwitch({
        state: sens_sht10,
        onColor: has_sht10 ? "success" : "warning",
        offColor: has_sht10 ? "danger" : "warning"
    });
    $("#sens_mcp3421").bootstrapSwitch({
        state: sens_mcp3421,
        onColor: has_mcp3421 ? "success" : "warning",
        offColor: has_mcp3421 ? "danger" : "warning"
    });

    $("#cfg_oled").bootstrapSwitch({
        state: cfg_oled,
        onColor: has_oled ? "success" : "warning",
        offColor: has_oled ? "danger" : "warning"
    });
    $("#cfg_ap").bootstrapSwitch('state', cfg_ap);
    $("#cfg_wifi").bootstrapSwitch('state', cfg_wifi);
    $("#cfg_debug").bootstrapSwitch('state', cfg_debug);
    $("#cfg_logger").bootstrapSwitch('state', cfg_logger);
    $("#cfg_rgb").bootstrapSwitch('state', cfg_rgb);
    $("#cfg_demo").bootstrapSwitch('state', cfg_demo);
    $("#cfg_tinfo").bootstrapSwitch('state', cfg_tinfo);
    $("#cfg_tinfo").bootstrapSwitch('disabled', true);
    $("#cfg_static").bootstrapSwitch('state', cfg_static);
}

// We clicked on a tab 
$('a[data-toggle=\"tab\"]').on('shown.bs.tab', function (e) {
    var target = $(e.target).attr("href")
    console.log('activated tab ' + target);

    // IE10, Firefox, Chrome, etc.
    if (history.pushState)
        window.history.pushState(null, null, target);
    else
        window.location.hash = target;

    if (target == '#tab_tinfo') {
        wsSend('$tinfo:' + $('#tinfo_sld_rfh').slider('getValue'));
        //$('#sensors_sld_rfh').redraw();
        //$('#si7021_chart').redraw();
        //$('#sht10_chart').redraw();

    } else if (target == '#tab_sensor') {
        wsSend('$sensors:' + $('#sensors_sld_rfh').slider('getValue'));
        $("#led_panel").slider('relayout');
        //$('#sensors_sld_rfh').redraw();
        //$('#si7021_chart').redraw();
        //$('#sht10_chart').redraw();

    } else if (target == '#tab_sys') {
        //$('#tab_sys_data').bootstrapTable('refresh',{silent:true, url:urls['system']});  
        wsSend('$system');
    } else if (target == '#tab_log') {
        //Populate the datagrid with log file content (from json)
        wsSend('$logger');
        //onBodyLoad
        //wsSend('$log');
    } else if (target == '#tab_edit') {
        $("#tab_edit_frm").attr("src", "/edit.htm");
        //onBodyLoad();
    } else if (target == '#tab_term') {
        setTimeout(function () {
            term.focus(true)
        }, 500);
    } else if (target == '#tab_fs') {
        refreshSpiffs();
        // Configuration Tab activation
    } else if (target == '#tab_cfg') {
        //socket.emit('config', true);
        wsSend('$config');
        $.getJSON(urls['config'], fillConfigForm)
            .fail(function (xhr, textStatus, errorThrown) {
                Notify(2, 'exclamation-triangle', 'warning', 'Error while getting configuration', xhr.status + ' ' + errorThrown);
            })

        //$('#tab_scan_data').bootstrapTable('refresh',{silent:true, showLoading:true, url:'/wifiscan.json'});  
    }
});

$('#tab_sys_data').on('load-success.bs.table', function (e, data) {
    console.log('#tab_sys_data loaded');
})
$('#tab_log_data').on('load-success.bs.table', function (e, data) {
    console.log('#tab_log_data loaded');
})
$('#tab_fs_data')
    .on('load-success.bs.table', function (e, data) {
        console.log('#tab_fs_data loaded');
    })
    .on('load-error.bs.table', function (e, status) {
        console.log('Event: load-error.bs.table');
    });

$('#btn_scan').click(function () {
    scanWifi();
});

$('#confirm-del').on('show.bs.modal', function (event) {
    var button = $(event.relatedTarget) // Button that triggered the modal
    var file2delete = button.data('file2delete') // Extract info from data-* attributes
    var modal = $(this)
    modal.find('#file2delete').text(file2delete)
    modal.find('#btn_delete').attr("file2delete", file2delete)
});

$('#btn_delete').click(function () {
    var $btn = $(this).button('...');

    $.post('/spiffs_op', {
            action: "delete",
            file: $btn.attr("file2delete")
        },
        function (msg, textStatus, xhr) {
            Notify(2, 'ok', 'success', 'File deleted', xhr.status + ' ' + msg);
        }).fail(
        function (xhr, textStatus, errorThrown) {
            Notify(4, 'remove', 'danger', 'Unable to delete', xhr.status + ' ' + errorThrown);
        });
    refreshSpiffs();
});

$('#file_cfg').change(function () {
    var $txt = $('#txt_load_cfg');
    var $btn = $('#btn_import');
    var ok = true;
    f = this.files[0];
    var name = f.name.toLowerCase();
    var size = f.size;
    var type = f.type;
    console.log('name=' + name);
    console.log('size' + size);
    console.log('type=' + type);

    if (name != "nrjmeter.config") {
        Notify(5, 'minus-circle', 'danger', 'Bad filename', 'Updated filename should be named nrjmeter.config');
        ok = false;
    }
    if (ok) {
        $btn.prop('disabled', false);
        $txt.attr('readonly', '');
        $txt.val(name);
        $txt.attr('readonly', 'readonly');
    } else {
        $btn.prop('disabled', true);
        $txt.attr('readonly', '');
        $txt.val('');
        $txt.attr('readonly', 'readonly');
    }
    return ok;
});

$('#btn_import').click(function () {
    if (f) {
        var r = new FileReader();
        r.onload = function (e) {
            var contents = e.target.result;
            fillConfigForm(JSON.parse(contents));
            $('#frm_config').trigger('submit');
            $('#txt_load_cfg').val('');
            $('#btn_import').prop('disabled', true);
        }
        r.readAsText(f);
    } else {
        alert("Failed to load file");
    }
});

$('#btn_export').click(function () {
    var exportFilename = 'nrjmeter.config';

    //Force disabled checkboxes to be sent
    $("#frm_config").find('input[type=checkbox]').each(function () {
        var checkbox = $(this);

        // add a hidden field with the same name before the checkbox with value = 0
        if (!checkbox.prop('checked')) {
            checkbox.clone()
                .prop('type', 'hidden')
                .addClass('ghost')
                .val("0")
                .insertBefore(checkbox);
        }
    });

    var sdata = $("#frm_config").serializeArray();

    //Remove ghost elements added
    $('.ghost').remove();

    var configFile = '{\r\n';
    for (var i = 0; i < sdata.length; i++) {
        if (isNaN(sdata[i].value) || sdata[i].value == "" || (!isNaN(sdata[i].value) && sdata[i].value.length > 1 && sdata[i].value[0] == "0")) {
            if (sdata[i].value == "on") {
                configFile += "\"" + sdata[i].name + "\"" + ": " + "1";
            } else {
                configFile += "\"" + sdata[i].name + "\"" + ": " + "\"" + sdata[i].value + "\"";
            }
        } else {
            configFile += "\"" + sdata[i].name + "\"" + ": " + sdata[i].value;
        }

        configFile += (i < sdata.length - 1 ? "," : "");
        configFile += '\r\n';
    }

    configFile += '}'

    var blob = new Blob([configFile], {
        type: 'text/csv;charset=utf-8;'
    });
    if (navigator.msSaveBlob) { // IE 10+
        navigator.msSaveBlob(blob, exportFilename);
    } else {
        var link = document.createElement("a");
        if (link.download !== undefined) { // feature detection
            // Browsers that support HTML5 download attribute
            var url = URL.createObjectURL(blob);
            link.setAttribute("href", url);
            link.setAttribute("download", exportFilename);
            link.style.visibility = 'hidden';
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
        }
    }

    return false;
});

$('#btn_rst').click(function () {
    $.post(urls['config_reset']);
    waitReboot();
    return false;
});

$('#btn_raz').click(function () {
    $.post(urls['factory_reset']);
    waitReboot();
    return false;
});

$('#btn_reboot').click(function () {
    $.post(urls['reset']);
    waitReboot();
    return false;
});

$('#tab_scan')
    .on('refresh.bs.table', function () {
        scanWifi();
    })
    .on('click-row.bs.table', function (e, name, args) {
        var $form = $('#tab_cfg');
        $('#ssid').val(name.ssid);
        $('#tab_scan').on('hidden.bs.modal', function () {
            $('#psk').select()
        });
        //setTimeout(function(){$('#psk').select()},500);  
        $('#tab_scan').modal('hide');
    });

$(document)
    .on('change', '.btn-file :file', function () {
        var input = $(this),
            numFiles = input.get(0).files ? input.get(0).files.length : 1,
            label = input.val().replace(/\\/g, '/').replace(/.*\//, '');
        input.trigger('fileselect', [numFiles, label]);
    })
    .on('hide.bs.collapse', '.card-collapse', function (e) {
        var $span = $(this).parents('.card').find('span.pull-right.fa');
        $span.removeClass('fa-chevron-up').addClass('fa-chevron-down');
    })
    .on('show.bs.collapse', '.card-collapse', function (e) {
        var $span = $(this).parents('.card').find('span.pull-right.fa');
        $span.removeClass('fa-chevron-down').addClass('fa-chevron-up');
    })
    .on('shown.bs.collapse', '.card-collapse', function (e) {
        if (e.currentTarget.id == 'col_sht10') {
            sht10_chart.resize();
            sht10_chart.update();
        } else if (e.currentTarget.id == 'col_si7021') {
            si7021_chart.resize();
            si7021_chart.update();
        } else if (e.currentTarget.id == 'col_mcp3421') {
            mcp3421_chart.resize();
            mcp3421_chart.update();
        } else if (e.currentTarget.id == 'col_sensors') {
            $("#cfg_led_bright").slider('relayout');
        }
    });

$('#frm_config').validator().on('submit', function (e) {
    // everything looks good!
    if (!e.isDefaultPrevented()) {
        var myForm = $("#frm_config").serialize();
        e.preventDefault();
        console.log("Form Submit=" + myForm);

        //Force disabled checkboxes to be sent
        $(this).find('input[type=checkbox]').each(function () {
            var checkbox = $(this);

            // add a hidden field with the same name before the checkbox with value = 0
            if (!checkbox.prop('checked')) {
                checkbox.clone()
                    .prop('type', 'hidden')
                    .addClass('ghost')
                    .val("off")
                    .insertBefore(checkbox);
            }
        });

        $.post(urls['config_form'], $("#frm_config").serialize())
            .done(function (msg, textStatus, xhr) {
                Notify(2, 'check', 'success', 'Configuration saved', xhr.status + ' ' + msg);
            })
            .fail(function (xhr, textStatus, errorThrown) {
                Notify(4, 'minus-circle', 'danger', 'Error while saving configuration', xhr.status + ' ' + errorThrown);
            });

        //Remove ghost elements added
        $('.ghost').remove();
    }
});

$('#file_fw').change(function () {
    var $txt = $('#txt_upload_fw');
    var $btn = $('#btn_upload_fw');
    var ok = true;
    var f = this.files[0];
    var name = f.name.toLowerCase();
    var size = f.size;
    var type = f.type;
    var html = 'Fichier:' + name + '&nbsp;&nbsp;type:' + type + '&nbsp;&nbsp;taille:' + size + ' octets'
    console.log('name=' + name);
    console.log('size' + size);
    console.log('type=' + type);

    $('#pgfw').removeClass('show').addClass('hide');
    $('#sfw').text(name + ' : ');

    if (!f.type.match('application/octet-stream') && !f.type.match('application/macbinary')) {
        Notify(3, 'minus-circle', 'danger', 'Bad file type', 'Updated file should be a binary file');
        ok = false;
    } else if (name != "nrjmeter.cpp.bin" && name != "nrjmeter.spiffs.bin") {
        Notify(5, 'minus-circle', 'danger', 'Bad filename', 'Updated filename should be named <ul><li>nrjmeter.cpp.bin (Firmware) or</li><li>nrjmeter.spiffs.bin (SPIFFS Filesystem)</li></ul>');
        ok = false;
    }
    if (ok) {
        $btn.removeClass('hide');
        if (name === "nrjmeter.cpp.bin") {
            label = 'Firmware Update';
        } else {
            label = 'SPIFFS Update';
        }
        $btn.val(label);
        $('#fw_info').html('<strong>' + label + '</strong> ' + html);
    } else {
        $txt.attr('readonly', '');
        $txt.val('');
        $txt.attr('readonly', 'readonly');
        $btn.addClass('hide');
    }
    return ok;
});

$('#btn_upload_fw').click(function () {
    var formData = new FormData($('#frm_fw')[0]);
    $.ajax({
        url: urls['update'],
        type: 'POST',
        data: formData,
        cache: false,
        contentType: false,
        processData: false,
        xhr: function () {
            var myXhr = $.ajaxSettings.xhr();
            if (myXhr.upload)
                myXhr.upload.addEventListener('progress', progressUpload, false);
            return myXhr;
        },
        beforeSend: function () {
            $('#pgfw').removeClass('hide');
        },
        success: function (msg, textStatus, xhr) {
            Notify(2, 'floppy-o', 'success', 'Upgrade sent successfull', '<strong>' + xhr.status + '</strong> ' + msg);
            waitReboot();
        },
        error: function (xhr, textStatus, errorThrown) {
            $('#pfw').removeClass('bg-success').addClass('bg-danger');
            Notify(4, 'minus-circle', 'danger', 'Error while upgradding file ' + name, '<strong>' + xhr.status + '</strong> ' + errorThrown);
        }
    });
});

function refreshPage() {
    var url = document.location.toString();
    var tab = url.split('#')[1];
    if (typeof (tab) === 'undefined')
        tab = 'tab_tinfo';
    //console.log('Tab activation='+tab);
    $('.nav-tabs a[href="#' + tab + '"]').tab('show');
    $('a[data-toggle="tab"][href="' + tab + '"]').trigger("shown.bs.tab");
    console.log('refreshPage(' + tab + ')')
    //$('.nav-tabs a[href=#'+tab+']').on('shown', function(e){window.location.hash=e.target.hash;});   
}

window.onload = function () {

    // Instanciate the terminal object
    term = $('#term').terminal(function (command, term) {
            if (command == '!help') {
                this.echo("available commands are [[b;cyan;]!help], [[b;cyan;]!log], [[b;cyan;]!nolog]\n" +
                    "each terminal commmand need to be prefixed by [[b;cyan;]!]\n" +
                    "if not they will be send and interpreted by NRJMeter\n" +
                    "examples: [[b;cyan;]!nolog]  disable log output to this console\n" +
                    "          [[b;cyan;]!log]    enable log output to this console");

            } else if (command == '!nolog') {
                this.echo("[[b;cyan;]Log disabled]");
                logs = false;
            } else if (command == '!log') {
                this.echo("[[b;cyan;]Log enabled]");
                logs = true;
            } else if (command == '!log?') {
                this.echo("[[b;cyan;]Log=" + logs + "]");
            } else {
                // Send raw command to client
                wsSend(command);
            }
        },
        // Default terminal settings and greetings
        {
            prompt: '[[b;red;]>]',
            checkArity: false,
            greetings: "Type [[b;cyan;]help] to see NRJMeter available commands\n" +
                "Type [[b;cyan;]!help] to see terminal commands"
        }
    );

    // open a web socket
    socksrv = 'ws://' + location.hostname + ':' + location.port + '/ws';
    //socksrv = 'ws://' + '192.168.1.45' + ':' + '80' + '/ws';
    console.log('socket server=' + socksrv);
    //ws = new WebSocket("ws://localhost:8081");
    //ws = new WebSocket(socksrv);
    ws = new ReconnectingWebSocket(socksrv);
    //ws.debug = true;

    // When Web Socket is connected
    ws.onopen = function (evt) {
        console.log('WS Connect');
        //Notify(1, 'eye-open', 'success', 'Connected to nrjmeter', '');
        //$('.nav-tabs a[href=#'+document.location.toString().split('#')[1]+']').tab('show');  
        $('#mdl_wait').modal('hide');
        $('#connect').text('connected').removeClass('badge-default').removeClass('badge-danger').addClass('badge-success');
        var srv = location.hostname;
        term.echo("WS[Connected] to [[;green;]ws:/" + srv + ':' + location.port + "/ws]");
        term.set_prompt("[[b;green;]" + srv + " #]");
        elapsed = 0;
        clearInterval(el_timer);
        refreshPage();
    };

    // When websocket is closed.
    ws.onclose = function (code, reason) {
        $('#connect').text('disconnected').removeClass('badge-success').removeClass('badge-default').addClass('badge-danger');
        term.error('WS[Disconnected] ' + code + ' ' + reason);
        term.set_prompt("[[b;red;]>]");
    };

    // When websocket message
    ws.onmessage = function (evt) {
        console.log('WS Received ' + evt.data);
        if (isJson(evt.data)) {
            console.log('WS Received JSON');
            var obj = eval("(" + evt.data + ")");
            var msg = obj.message;
            var data = obj.data;

            if (logs == true) {
                var str = ts() + "WS[" + msg + "] " + JSON.stringify(data);
                str = str.replace(/\]/g, "&#93;");
                term.echo("[[;darkgrey;]" + str + "]");
            }
            if (msg == 'tinfo')
                refreshTinfo(data);
            if (msg == 'sensors')
                refreshSensors(data);
            if (msg == 'system')
                refreshSystem(data);
            if (msg == 'logger')
                refreshLogger(data);
            if (msg == 'config') {
                $('#led_panel').slider('setValue', data[0].ledpanel)
                console.log("WS received config ledpanel=" + data[0].ledpanel);
            }
            if (msg == 'log' && logs == true) {
                data.replace(/\]/g, "&#93;");
                term.echo("[[;darkgrey;]" + ts() + data + "]");
            }
        } else {
            // Raw Data (mainly response)
            console.log('WS Received RAW');
            var str = evt.data;
            str = str.replace(/\]/g, "&#93;");
            term.echo("[[;darkgrey;]" + str + "]");
        }
    };

    // When Web Socket error
    ws.onerror = function (evt) {
        term.error(ts() + 'WS[error]');
    };

    $("#tinfo_sld_rfh")
        .slider({
            id: "tinfo_slider_rfh",
            formatter: function (v) {
                return v + ' seconds';
            }
        })
        .on('slideStop', function () {
            wsSend('$tinfo:' + $('#tinfo_sld_rfh').slider('getValue'));
        });

    $("#sensors_sld_rfh")
        .slider({
            id: "sensors_slider_rfh",
            formatter: function (v) {
                return v + ' seconds';
            }
        })
        .on('slideStop', function () {
            wsSend('$sensors:' + $('#sensors_sld_rfh').slider('getValue'));
        });

    $("#led_panel")
        .slider({
            id: "slider_led_panel",
            formatter: function (v) {
                return v + '%';
            }
        })
        .on('slideStop', function () {
            wsSend('$lpnl:' + $('#led_panel').slider('getValue'));
        });

    $("#cfg_led_bright")
        .on('slideStop', function () {
            wsSend('$rgbb:' + $('#cfg_led_bright').slider('getValue'));
        });

    //$("#sens_si7021").bootstrapSwitch({onColor:"warning", offColor:"warning" });
    //$("#sens_sht10").bootstrapSwitch({ onColor: "warning", offColor:"warning" });
    //$("#sens_mcp3421").bootstrapSwitch({ onColor: "warning", offColor:"warning" });

    $("#cfg_debug").bootstrapSwitch({
        onColor: "success",
        offColor: "danger"
    });
    $("#cfg_logger").bootstrapSwitch({
        onColor: "success",
        offColor: "danger"
    });
    $("#cfg_rgb").bootstrapSwitch({
        onColor: "success",
        offColor: "danger"
    });
    //$("#cfg_oled").bootstrapSwitch({ onColor:"success", offColor:"danger"});
    $("#cfg_ap").bootstrapSwitch({
        onColor: "success",
        offColor: "danger"
    });
    $("#cfg_wifi").bootstrapSwitch({
        onColor: "success",
        offColor: "danger"
    });
    $("#cfg_static").bootstrapSwitch({
        onColor: "success",
        offColor: "danger"
    });

    var ctx_tinfo_papp_iinst = document.getElementById("tinfo_papp_iinst_chart").getContext('2d');
    tinfo_chart = new Chart(ctx_tinfo_papp_iinst, {
        "type": "bar",
        "data": {
            "labels": ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""],
            "datasets": [{
                "label": "Power (W)",
                "data": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                yAxisID: 'W',
                type: 'line',
                "borderColor": "rgba(77, 204, 51, 1)",
                "backgroundColor": "rgba(77, 204, 51, 0.2)"
            }, {
                "label": "Energy (A)",
                "data": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                yAxisID: 'A',
                "type": "bar",
                "fill": true,
                "borderColor": "rgba(64, 149, 191, 1)",
                "backgroundColor": "rgba(64, 149, 191, 0.2)"
            }]
        },
        "options": {
            responsive: true,
            "scales": {
                "yAxes": [
                    {
                        "id": 'W',
                        "type": 'linear',
                        "position": 'left',
                        "gridLines": {
                            "display": false
                        },
                        "ticks": {
                            "beginAtZero": true
                        }
                },
                    {
                        "id": 'A',
                        "type": 'linear',
                        "position": 'right',
                        "gridLines": {
                            "display": false
                        },
                        "ticks": {
                            "beginAtZero": true
                        }
                }]
            }
        }
    });

    ctx_mcp3421 = $("#mcp3421_chart").get(0).getContext("2d");
    mcp3421_chart = new Chart(ctx_mcp3421, {
        "type": "bar",
        "data": {
            "labels": ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""],
            "datasets": [{
                "label": "Power (W)",
                "data": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                yAxisID: 'W',
                type: 'line',
                "borderColor": "rgba(77, 204, 51, 1)",
                "backgroundColor": "rgba(77, 204, 51, 0.2)"
            }]
        },
        "options": {
            responsive: true,
            "scales": {
                "yAxes": [
                    {
                        "id": 'W',
                        "type": 'linear',
                        "position": 'left',
                        "gridLines": {
                            "display": false
                        },
                        "ticks": {
                            "min": 0,
                            "beginAtZero": true
                        }
                }]
            }
        }
    });

    ctx_si = $("#si7021_chart").get(0).getContext("2d");
    si7021_chart = new Chart(ctx_si, {
        "type": "bar",
        "data": {
            "labels": ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""],
            "datasets": [{
                "label": "Temperature (°C)",
                "data": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                yAxisID: 'C',
                type: 'line',
                "borderColor": "rgba(77, 204, 51, 1)",
                "backgroundColor": "rgba(77, 204, 51, 0.2)"
            }, {
                "label": "Humidity (%)",
                "data": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                yAxisID: 'P',
                "type": "bar",
                "fill": true,
                "borderColor": "rgba(64, 149, 191, 1)",
                "backgroundColor": "rgba(64, 149, 191, 0.2)"
            }]
        },
        "options": {
            responsive: true,
            "scales": {
                "yAxes": [
                    {
                        "id": 'C',
                        "type": 'linear',
                        "position": 'left',
                        "gridLines": {
                            "display": false
                        },
                        "ticks": {
                            "min": 0,
                            "max": 100,
                            "beginAtZero": true
                        }
                },
                    {
                        "id": 'P',
                        "type": 'linear',
                        "position": 'right',
                        "gridLines": {
                            "display": false
                        },
                        "ticks": {
                            "min": 0,
                            "max": 100,
                            "beginAtZero": true
                        }
                }]
            }
        }
    });

    ctx_sh = $("#sht10_chart").get(0).getContext("2d");
    sht10_chart = new Chart(ctx_sh, {
        "type": "bar",
        "data": {
            "labels": ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""],
            "datasets": [{
                "label": "Temperature (°C)",
                "data": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                yAxisID: 'C',
                type: 'line',
                "borderColor": "rgba(77, 204, 51, 1)",
                "backgroundColor": "rgba(77, 204, 51, 0.2)"
            }, {
                "label": "Humidity (%)",
                "data": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                yAxisID: 'P',
                "type": "bar",
                "fill": true,
                "borderColor": "rgba(64, 149, 191, 1)",
                "backgroundColor": "rgba(64, 149, 191, 0.2)"
            }]
        },
        "options": {
            responsive: true,
            "scales": {
                "yAxes": [
                    {
                        "id": 'C',
                        "type": 'linear',
                        "position": 'left',
                        "gridLines": {
                            "display": false
                        },
                        "ticks": {
                            "min": 0,
                            "max": 100,
                            "beginAtZero": true
                        }
                },
                    {
                        "id": 'P',
                        "type": 'linear',
                        "position": 'right',
                        "gridLines": {
                            "display": false
                        },
                        "ticks": {
                            "min": 0,
                            "max": 100,
                            "beginAtZero": true
                        }
                }]
            }
        }
    });
    /*
	var data = {
    labels: [],
    datasets: [
        {
            label: "Temperature",
            fillColor: "rgba(77,204,51,0.2)",
            strokeColor: "rgba(77,204,51,1)",
            pointColor: "rgba(77,204,51,1)",
            pointStrokeColor: "#fff",
            pointHighlightFill: "#fff",
            pointHighlightStroke: "rgba(77,204,51,1)"
	   },
        {
            label: "Humidity",
            fillColor: "rgba(64,149,191,0.2)",
            strokeColor: "rgba(64,149,191,1)",
            pointColor: "rgba(64,149,191,1)",
            pointStrokeColor: "#fff",
            pointHighlightFill: "#fff",
            pointHighlightStroke: "rgba(64,149,191,1)"
		    }
	    ],
    data: [{
            type: "line",
            axisYindex: 0
	      },
        {
            type: "column",
            axisYIndex: 1
        }
    ]
};
	
  */
    /*
      mcp3421_gauge = new Gauge(document.getElementById('mcp3421_gauge')).setOptions( {
    												  lines: 12,
    												  angle: 0.15,
    												  lineWidth: 0.44,
    												  pointer: { length: 0.7, strokeWidth: 0.035, color: '#000000' },
    												  limitMax: 'false', 
    												  percentColors: [[0.0, "#a9d70b" ], [0.50, "#f9c802"], [1.0, "#ff0000"]], // !!!!
    												  strokeColor: '#E0E0E0',
    												  generateGradient: true });

    	mcp3421_gauge.maxValue = 500;
    	mcp3421_gauge.setMinValue(0);  // Prefer setter over gauge.minValue = 0
    	mcp3421_gauge.set(1); // set actual value
    */

    //refreshSensors();
    //refreshPage();
}
