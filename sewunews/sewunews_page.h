const char WEBPAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>SEWU NEWS CONFIG</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif;background:#0d1117;color:#c9d1d9;min-height:100vh;padding:20px}
.container{max-width:800px;margin:0 auto}
.header{background:#161b22;border:1px solid #30363d;border-radius:6px;padding:24px;margin-bottom:16px}
.header h1{font-size:24px;font-weight:600;margin-bottom:4px}
.header p{color:#8b949e;font-size:14px}
.card{background:#161b22;border:1px solid #30363d;border-radius:6px;padding:16px;margin-bottom:16px}
.card-title{font-size:14px;font-weight:600;margin-bottom:12px;color:#c9d1d9}
.info-box{background:#0d1117;border:1px solid #30363d;border-radius:6px;padding:12px;margin-bottom:16px;font-size:14px}
.info-box strong{color:#58a6ff;display:block;margin-bottom:4px;font-size:12px}
select,input[type="text"],input[type="password"],input[type="date"],input[type="time"]{width:100%;padding:8px 12px;font-size:14px;background:#0d1117;border:1px solid #30363d;border-radius:6px;color:#c9d1d9;margin-bottom:8px}
select:focus,input:focus{outline:none;border-color:#58a6ff}
.grid-2{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.slider-box{display:flex;align-items:center;gap:12px;margin-bottom:8px}
.slider{flex:1;height:6px;border-radius:3px;background:#21262d;outline:none;-webkit-appearance:none}
.slider::-webkit-slider-thumb{-webkit-appearance:none;width:16px;height:16px;border-radius:50%;background:#58a6ff;cursor:pointer}
.slider::-moz-range-thumb{width:16px;height:16px;border-radius:50%;background:#58a6ff;cursor:pointer;border:none}
.speed-val{min-width:50px;padding:4px 8px;background:#21262d;border-radius:4px;text-align:center;font-size:12px;color:#58a6ff}
.btn{background:#238636;color:#fff;border:none;padding:10px 16px;font-size:14px;font-weight:600;border-radius:6px;cursor:pointer;width:100%;transition:background 0.2s}
.btn:hover{background:#2ea043}
.btn:active{background:#26a641}
.status{padding:10px;border-radius:6px;text-align:center;font-size:13px;font-weight:500;margin-top:8px;display:none}
.status.ok{background:#1a7f37;color:#fff;display:block}
.status.err{background:#da3633;color:#fff;display:block}
.news-list{max-height:400px;overflow-y:auto;border:1px solid #30363d;border-radius:6px;background:#0d1117}
.news-item{padding:12px;border-bottom:1px solid #21262d;cursor:pointer;transition:background 0.1s}
.news-item:hover{background:#161b22}
.news-item:last-child{border-bottom:none}
.news-item .src{font-size:12px;color:#58a6ff;margin-bottom:4px}
.news-item .txt{font-size:13px;color:#c9d1d9}
small{color:#8b949e;font-size:12px}
</style>
</head>
<body>
<div class="container">
<div class="header">
<h1>SEWU NEWS</h1>
<p>Configuration Panel</p>
</div>

<div class="info-box">
<strong>DEVICE IP</strong>
<span id="ip">Loading...</span>
</div>

<div class="card">
<div class="card-title">WiFi Setup</div>
<input type="text" id="wifiSSID" placeholder="WiFi SSID">
<input type="password" id="wifiPassword" placeholder="WiFi Password">
<button class="btn" onclick="saveWiFi()">Save WiFi & Reboot</button>
<div class="status" id="sWifi"></div>
</div>

<div class="card">
<div class="card-title">Scroll Speed</div>
<div class="slider-box">
<input type="range" min="20" max="100" value="40" class="slider" id="spd" oninput="updSpd()">
<div class="speed-val" id="spdV">40ms</div>
</div>
<small>Left = Fast, Right = Slow</small>
</div>

<div class="card">
<div class="card-title">Set Date & Time</div>
<div class="grid-2">
<input type="date" id="rtcD">
<input type="time" id="rtcT">
</div>
<button class="btn" onclick="setRTC()">Set RTC</button>
<div class="status" id="sRTC"></div>
</div>

<div class="card">
<div class="card-title">News Source - Media</div>
<select id="med">
<option value="">-- Select Media --</option>
<option value="media:detik.com">Detik.com</option>
<option value="media:kompas.com">Kompas.com</option>
<option value="media:kompas.id">Kompas.id</option>
<option value="media:nu.or.id">NU Online</option>
<option value="media:tempo.co">Tempo.co</option>
<option value="media:cnnindonesia.com">CNN Indonesia</option>
<option value="media:liputan6.com">Liputan6.com</option>
<option value="media:antaranews.com">Antara News</option>
<option value="media:tribunnews.com">Tribun News</option>
<option value="media:republika.co.id">Republika</option>
<option value="media:okezone.com">Okezone</option>
<option value="media:kumparan.com">Kumparan</option>
<option value="media:suara.com">Suara.com</option>
<option value="media:merdeka.com">Merdeka.com</option>
<option value="media:mediaindonesia.com">Media Indonesia</option>
<option value="media:jpnn.com">JPNN</option>
<option value="media:idntimes.com">IDN Times</option>
<option value="media:bisnis.com">Bisnis.com</option>
<option value="media:kontan.co.id">Kontan</option>
<option value="media:cnbcindonesia.com">CNBC Indonesia</option>
<option value="media:katadata.co.id">Katadata</option>
<option value="media:techinasia.com">Tech in Asia</option>
<option value="media:kapanlagi.com">Kapanlagi</option>
<option value="media:bola.com">Bola.com</option>
<option value="media:bola.net">Bola.net</option>
</select>
</div>

<div class="card">
<div class="card-title">News Source - Category</div>
<select id="cat">
<option value="">-- Select Category --</option>
<option value="headline">Top Headlines</option>
<option value="category:POLITICS">Politik</option>
<option value="category:BUSINESS">Bisnis & Ekonomi</option>
<option value="category:TECHNOLOGY">Teknologi</option>
<option value="category:SCIENCE">Sains</option>
<option value="category:HEALTH">Kesehatan</option>
<option value="category:SPORTS">Olahraga</option>
<option value="category:ENTERTAINMENT">Hiburan & Lifestyle</option>
<option value="category:WORLD">Internasional</option>
<option value="category:NATION">Nasional</option>
<option value="search:otomotif">Otomotif</option>
<option value="search:travel">Travel</option>
<option value="search:kuliner">Kuliner</option>
</select>
</div>

<div class="card">
<div class="card-title">Custom Search</div>
<input type="text" id="cus" placeholder="Enter keyword: jokowi, gempa, etc...">
</div>

<button class="btn" onclick="save()">Save News Source</button>
<div class="status" id="sNews"></div>

<div class="card" style="margin-top:24px">
<div class="card-title">Live News Preview</div>
<div class="news-list" id="nList">Loading...</div>
</div>
</div>

<script>
function $(id){return document.getElementById(id)}
function stat(id,m,ok){var s=$(id);s.textContent=m;s.className='status '+(ok?'ok':'err');setTimeout(()=>s.style.display='none',3000)}

function updSpd(){
  var v=$('spd').value;
  $('spdV').textContent=v+'ms';
  fetch('/api/speed?speed='+v,{method:'POST'});
}

function saveWiFi(){
  var s=$('wifiSSID').value.trim();
  var p=$('wifiPassword').value.trim();
  if(!s){stat('sWifi','SSID required!',false);return}
  fetch('/api/wifi',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({wifiSSID:s,wifiPassword:p})})
  .then(r=>r.json()).then(d=>stat('sWifi',d.ok?'Saved! Rebooting...':'Failed',d.ok));
}

function setRTC(){
  var d=$('rtcD').value;
  var t=$('rtcT').value;
  if(!d||!t){stat('sRTC','Date & time required!',false);return}
  var p=d.split('-');
  var tp=t.split(':');
  fetch('/api/setRTC',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({year:parseInt(p[0]),month:parseInt(p[1]),day:parseInt(p[2]),hour:parseInt(tp[0]),minute:parseInt(tp[1])})})
  .then(r=>r.json()).then(d=>stat('sRTC',d.ok?'RTC updated!':'Failed',d.ok));
}

function loadNews(){
  fetch('/api/news').then(r=>r.json()).then(d=>{
    var l=$('nList');
    if(d.count==0){l.innerHTML='<div style="padding:12px;text-align:center;color:#8b949e">No news</div>';return}
    var h='';
    d.items.forEach((n,i)=>{
      var ok=n.link&&n.link.length>5;
      var c=ok?`onclick='window.open("${n.link.replace(/"/g,'&quot;')}","_blank")'`:'style="cursor:default;opacity:0.5"';
      var ic=ok?'🔗 ':'';
      h+=`<div class="news-item" ${c}><div class="src">${ic}${i+1}. ${n.source}</div><div class="txt">${n.title}</div></div>`;
    });
    l.innerHTML=h;
  }).catch(e=>$('nList').innerHTML='<div style="padding:12px;color:#f85149">Error loading</div>');
}

function load(){
  fetch('/api/data').then(r=>r.json()).then(d=>{
    $('ip').textContent=d.ip;
    $('wifiSSID').value=d.wifiSSID;
    $('spd').value=d.scrollSpeed;
    $('spdV').textContent=d.scrollSpeed+'ms';
    var n=new Date();
    $('rtcD').value=n.toISOString().split('T')[0];
    $('rtcT').value=n.toTimeString().substring(0,5);
    var s=d.newsSource;
    if(s.startsWith('media:')){$('med').value=s;$('cat').value='';$('cus').value=''}
    else if(s.startsWith('search:')){var k=s.substring(7);if($('cat').querySelector(`option[value="search:${k}"]`))$('cat').value=s;else $('cus').value=k;$('med').value=''}
    else{$('cat').value=s;$('med').value='';$('cus').value=''}
  });
  loadNews();
}

function save(){
  var m=$('med').value;
  var c=$('cat').value;
  var u=$('cus').value.trim();
  var src='headline';
  if(u)src='search:'+u;else if(m)src=m;else if(c)src=c;
  fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({newsSource:src})})
  .then(r=>r.json()).then(d=>{stat('sNews',d.ok?'Saved! Updating...':'Failed',d.ok);if(d.ok)setTimeout(loadNews,11000)});
}

$('med').onchange=()=>{if($('med').value){$('cat').value='';$('cus').value=''}};
$('cat').onchange=()=>{if($('cat').value){$('med').value='';$('cus').value=''}};
$('cus').oninput=()=>{if($('cus').value){$('med').value='';$('cat').value=''}};
setInterval(loadNews,30000);
window.onload=load;
</script>
</body>
</html>
)=====";
