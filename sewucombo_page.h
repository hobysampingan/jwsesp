/*
 * SEWU COMBO - Web Interface v1.0
 * Dirty tracking: Only load once, no override while editing
 */

const char webpage[] PROGMEM = R"=====(
<!DOCTYPE HTML><html lang="id"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>SEWU COMBO</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{--bg:#0a0a0f;--card:#12121a;--input:#1a1a24;--border:#2a2a3a;--accent:#6366f1;--accent2:#818cf8;--ok:#22c55e;--text:#f4f4f5;--dim:#a1a1aa}
body{font-family:system-ui,sans-serif;background:var(--bg);color:var(--text);padding:16px;max-width:480px;margin:0 auto}
.hd{background:linear-gradient(135deg,var(--accent),var(--accent2));padding:20px;text-align:center;border-radius:16px;margin-bottom:16px}
.hd h1{font-size:20px;font-weight:800}
.hd p{font-size:12px;opacity:.8;margin-top:4px}
.clk{background:var(--card);padding:16px;border-radius:12px;text-align:center;margin-bottom:16px}
.clk .time{font-size:32px;font-family:monospace;font-weight:700;color:var(--accent2)}
.clk .date{font-size:12px;color:var(--dim);margin-top:4px}
.tb{display:grid;grid-template-columns:repeat(4,1fr);gap:6px;margin-bottom:16px}
.tb button{padding:10px 6px;border:none;background:var(--card);color:var(--dim);border-radius:8px;font-weight:600;font-size:11px;cursor:pointer}
.tb button.ac{background:var(--accent);color:#fff}
.sc{background:var(--card);border-radius:14px;padding:16px;margin-bottom:16px}
.st{font-weight:700;margin-bottom:14px;display:flex;align-items:center;gap:8px;font-size:14px}
.st span{font-size:18px}
.fg{margin-bottom:12px}
.fl{display:block;font-size:11px;color:var(--dim);margin-bottom:6px;text-transform:uppercase}
.fi{width:100%;padding:12px;background:var(--input);border:1px solid var(--border);border-radius:10px;color:var(--text);font-size:14px}
.fi:focus{outline:none;border-color:var(--accent)}
textarea.fi{min-height:50px;resize:vertical}
.tg{display:flex;align-items:center;justify-content:space-between;padding:8px 12px;background:var(--input);border-radius:8px;margin-bottom:6px}
.tg label{font-weight:600;font-size:12px}
.sw{position:relative;width:40px;height:22px}
.sw input{display:none}
.sw span{position:absolute;inset:0;background:var(--border);border-radius:11px;cursor:pointer;transition:.3s}
.sw span:before{content:'';position:absolute;width:16px;height:16px;left:3px;top:3px;background:#fff;border-radius:50%;transition:.3s}
.sw input:checked~span{background:var(--ok)}
.sw input:checked~span:before{transform:translateX(18px)}
.sg{margin-bottom:12px}
.sgh{display:flex;justify-content:space-between;margin-bottom:6px}
.sgl{font-size:10px;color:var(--dim)}
.sgv{font-size:14px;font-weight:700;color:var(--accent2)}
.sl{width:100%;height:6px;background:var(--input);border-radius:3px;appearance:none;cursor:pointer}
.sl::-webkit-slider-thumb{appearance:none;width:18px;height:18px;background:var(--accent2);border-radius:50%;cursor:pointer}
.btn{width:100%;padding:14px;border:none;border-radius:10px;font-weight:700;cursor:pointer;background:linear-gradient(135deg,var(--accent),var(--accent2));color:#fff;font-size:13px}
.pn{display:none}.pn.ac{display:block}
.ts{position:fixed;bottom:20px;left:50%;transform:translateX(-50%);background:var(--ok);color:#fff;padding:12px 24px;border-radius:10px;font-weight:600;display:none;z-index:100}
.ts.sh{display:block}
select.fi{appearance:none;background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='12' height='12' fill='%23a1a1aa'%3E%3Cpath d='M6 9L1 4h10z'/%3E%3C/svg%3E");background-repeat:no-repeat;background-position:right 12px center}
.mdg{display:grid;grid-template-columns:repeat(3,1fr);gap:6px;margin-bottom:14px}
.mdb{padding:12px 8px;background:var(--input);border:2px solid var(--border);border-radius:10px;color:var(--dim);font-size:11px;font-weight:600;cursor:pointer;text-align:center}
.mdb.ac{border-color:var(--accent);background:rgba(99,102,241,.15);color:var(--accent2)}
.mdb span{display:block;font-size:16px;margin-bottom:2px}
</style></head><body>

<div class="hd"><h1>🔊 SEWU COMBO</h1><p>Jam + Running Text v1.0</p></div>

<div class="clk">
<div class="time" id="clk">--:--:--</div>
<div class="date" id="dt">-- --- ----</div>
</div>

<div class="tb">
<button class="ac" onclick="sp('t',this)">📝 Teks</button>
<button onclick="sp('d',this)">🖥️ Display</button>
<button onclick="sp('c',this)">⏰ Waktu</button>
<button onclick="sp('w',this)">📶 WiFi</button>
</div>

<div class="pn ac" id="pn-t">
<div class="sc">
<div class="st"><span>🏷️</span>Nama</div>
<input type="text" class="fi" id="nm" maxlength="63" placeholder="SEWU AUDIO">
</div>
<div class="sc">
<div class="st"><span>📝</span>Running Text</div>
<div class="tg"><label>Info 1</label><label class="sw"><input type="checkbox" id="e1" checked><span></span></label></div>
<textarea class="fi" id="i1" maxlength="199" placeholder="Teks 1..."></textarea>
<div class="tg"><label>Info 2</label><label class="sw"><input type="checkbox" id="e2" checked><span></span></label></div>
<textarea class="fi" id="i2" maxlength="199" placeholder="Teks 2..."></textarea>
<div class="tg"><label>Info 3</label><label class="sw"><input type="checkbox" id="e3" checked><span></span></label></div>
<textarea class="fi" id="i3" maxlength="199" placeholder="Teks 3..."></textarea>
<div class="tg"><label>Info 4</label><label class="sw"><input type="checkbox" id="e4"><span></span></label></div>
<textarea class="fi" id="i4" maxlength="199" placeholder="Teks 4..."></textarea>
<div class="tg"><label>Info 5</label><label class="sw"><input type="checkbox" id="e5"><span></span></label></div>
<textarea class="fi" id="i5" maxlength="199" placeholder="Teks 5..."></textarea>
<button class="btn" style="margin-top:12px" onclick="sT()">💾 Simpan Teks</button>
</div>
</div>

<div class="pn" id="pn-d">
<div class="sc">
<div class="st"><span>🖥️</span>Display Mode</div>
<div class="mdg">
<button class="mdb" id="m0" onclick="sM(0)"><span>🕐</span>Jam</button>
<button class="mdb" id="m1" onclick="sM(1)"><span>📝</span>Teks</button>
<button class="mdb ac" id="m2" onclick="sM(2)"><span>🔄</span>Auto</button>
</div>
<div class="fg" id="jdG">
<label class="fl">Durasi Jam (detik)</label>
<input type="number" class="fi" id="jd" min="5" max="60" value="10">
</div>
<div class="tg"><label>Format 24 Jam</label><label class="sw"><input type="checkbox" id="f24" checked><span></span></label></div>
<div class="sg">
<div class="sgh"><span class="sgl">Kecepatan</span><span class="sgv" id="spV">35ms</span></div>
<input type="range" class="sl" id="sp" min="15" max="80" value="35" oninput="uSp()">
</div>
<div class="sg">
<div class="sgh"><span class="sgl">Kecerahan</span><span class="sgv" id="brV">100</span></div>
<input type="range" class="sl" id="cr" min="20" max="255" value="100" oninput="uBr()">
</div>
<button class="btn" onclick="sD()">💾 Simpan Display</button>
</div>
</div>

<div class="pn" id="pn-c">
<div class="sc">
<div class="st"><span>⏰</span>Atur Waktu</div>
<div class="fg">
<label class="fl">Tanggal</label>
<input type="date" class="fi" id="di">
</div>
<div class="fg">
<label class="fl">Jam</label>
<input type="time" class="fi" id="ti">
</div>
<button class="btn" onclick="sTi()">💾 Atur Waktu</button>
</div>
<div class="sc">
<div class="st"><span>🔔</span>Alarm</div>
<div class="tg"><label>Aktifkan</label><label class="sw"><input type="checkbox" id="ae"><span></span></label></div>
<div class="fg"><label class="fl">Waktu</label><input type="time" class="fi" id="at"></div>
<div class="fg"><label class="fl">Durasi (detik)</label><input type="number" class="fi" id="ad" min="5" max="300" placeholder="60"></div>
<div class="fg"><label class="fl">Nada</label>
<select class="fi" id="an">
<option value="0">Tunggal (Beep)</option>
<option value="1">Ganda (Beep-Beep)</option>
<option value="2">Panjang (Beeeeep)</option>
<option value="3">Sirine (Ni-Nu-Ni-Nu)</option>
<option value="4">Pesan Masuk (Nokia)</option>
</select>
</div>
<button class="btn" onclick="sAl()">💾 Simpan Alarm</button>
</div>
</div>

<div class="pn" id="pn-w">
<div class="sc">
<div class="st"><span>📶</span>WiFi AP</div>
<div class="fg">
<label class="fl">Nama WiFi</label>
<input type="text" class="fi" id="ws" maxlength="31" placeholder="SEWU AUDIO">
</div>
<div class="fg">
<label class="fl">Password (min 8)</label>
<input type="text" class="fi" id="wp" maxlength="31" placeholder="********">
</div>
<button class="btn" onclick="sW()">💾 Simpan & Restart</button>
</div>
</div>

<div class="ts" id="ts">✅ Tersimpan!</div>

<script>
var D={},L=0,cM=2;
function $(i){return document.getElementById(i)}
function p(n){return n<10?'0'+n:n}

function init(){lD();setInterval(lD,3000)}

function lD(){
fetch('/api/data').then(r=>r.json()).then(d=>{
D=d;
$('clk').textContent=p(d.hour)+':'+p(d.minute)+':'+p(d.second);
$('dt').textContent=d.day+' '+d.dayName+' / '+d.temp+'°C';
if(!L){uUI();L=1}
}).catch(()=>{})
}

function uUI(){
$('nm').value=D.nama||'';
$('i1').value=D.info1||'';$('i2').value=D.info2||'';$('i3').value=D.info3||'';$('i4').value=D.info4||'';$('i5').value=D.info5||'';
$('e1').checked=D.enable1==1;$('e2').checked=D.enable2==1;$('e3').checked=D.enable3==1;$('e4').checked=D.enable4==1;$('e5').checked=D.enable5==1;
$('ws').value=D.ssid||'';
$('cr').value=D.brightness||100;$('sp').value=D.speed||35;
cM=D.displayMode||2;$('jd').value=D.jamDuration||10;
cM=D.displayMode||2;$('jd').value=D.jamDuration||10;
$('f24').checked=D.format24h==1;
$('ae').checked=D.alarmEnable==1;$('at').value=p(D.alarmHour)+':'+p(D.alarmMinute);$('ad').value=D.alarmDuration||60;$('an').value=D.alarmTune||0;
uBr();uSp();uMB()
}

function uSp(){$('spV').textContent=$('sp').value+'ms'}
function uBr(){$('brV').textContent=$('cr').value}

function sM(m){cM=m;uMB()}
function uMB(){
for(var i=0;i<=2;i++)$('m'+i).classList.toggle('ac',cM==i);
$('jdG').style.display=cM==2?'block':'none'
}

function sp(p,b){
document.querySelectorAll('.tb button').forEach(t=>t.classList.remove('ac'));
document.querySelectorAll('.pn').forEach(p=>p.classList.remove('ac'));
b.classList.add('ac');$('pn-'+p).classList.add('ac')
}

function sT2(m){var t=$('ts');t.textContent=m;t.classList.add('sh');setTimeout(()=>t.classList.remove('sh'),2500)}

function sT(){
var d={nama:$('nm').value,info1:$('i1').value,info2:$('i2').value,info3:$('i3').value,info4:$('i4').value,info5:$('i5').value,
enable1:$('e1').checked?1:0,enable2:$('e2').checked?1:0,enable3:$('e3').checked?1:0,enable4:$('e4').checked?1:0,enable5:$('e5').checked?1:0};
pJ('/api/info',d,()=>sT2('✅ Teks disimpan!'))
}

function sD(){
var d={brightness:parseInt($('cr').value),speed:parseInt($('sp').value),displayMode:cM,separator:0,jamDuration:parseInt($('jd').value),format24h:$('f24').checked?1:0};
pJ('/api/display',d,()=>sT2('✅ Display disimpan!'))
}

function sTi(){
var d=$('di').value,t=$('ti').value;
if(d||t)location.href='/?'+(d?'date='+d+'&':'')+(t?'time='+t:'');
}

function sW(){
var p=$('wp').value;
if(p.length>0&&p.length<8){alert('Password min 8!');return}
pJ('/api/wifi',{ssid:$('ws').value,password:p},()=>sT2('✅ Restart...'))
}

function sAl(){
var t=$('at').value.split(':');
var d={alarmEnable:$('ae').checked?1:0,alarmHour:parseInt(t[0]),alarmMinute:parseInt(t[1]),alarmDuration:parseInt($('ad').value),alarmTune:parseInt($('an').value)};
pJ('/api/alarm',d,()=>sT2('✅ Alarm disimpan!'))
}

function pJ(u,d,c){
fetch(u,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(d)}).then(r=>{if(r.ok&&c)c()})
}

window.onload=init
</script>
</body></html>
)=====";
