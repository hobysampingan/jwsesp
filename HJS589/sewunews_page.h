const char WEBPAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>SEWU NEWS CONFIG</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px}
.container{max-width:800px;margin:0 auto;background:#fff;border-radius:20px;box-shadow:0 20px 60px rgba(0,0,0,0.3);overflow:hidden}
.header{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:#fff;padding:30px;text-align:center}
.header h1{font-size:28px;margin-bottom:5px}
.header p{opacity:0.9;font-size:14px}
.content{padding:30px}
.section{margin-bottom:25px}
.section-title{font-size:16px;font-weight:bold;color:#333;margin-bottom:10px;display:flex;align-items:center}
.info-box{background:#f8f9fa;border-left:4px solid #667eea;padding:15px;border-radius:8px;margin-bottom:20px}
.info-box strong{color:#667eea;display:block;margin-bottom:5px}
select,input[type="text"]{width:100%;padding:12px;font-size:15px;border:2px solid #e0e0e0;border-radius:10px;background:#fff;transition:all 0.3s}
select:focus,input:focus{outline:none;border-color:#667eea}
.btn{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:#fff;border:none;padding:15px 30px;font-size:16px;border-radius:10px;cursor:pointer;width:100%;margin-top:20px;font-weight:bold;box-shadow:0 4px 15px rgba(102,126,234,0.4);transition:all 0.3s}
.btn:hover{transform:translateY(-2px);box-shadow:0 6px 20px rgba(102,126,234,0.6)}
.btn:active{transform:translateY(0)}
.status{padding:12px;border-radius:8px;text-align:center;font-weight:bold;margin-top:15px;display:none}
.status.success{background:#d4edda;color:#155724;display:block}
.status.error{background:#f8d7da;color:#721c24;display:block}
.news-list{max-height:400px;overflow-y:auto;border:1px solid #e0e0e0;border-radius:10px;padding:15px;background:#fafafa}
.news-item{background:#fff;padding:10px;margin-bottom:8px;border-radius:6px;border-left:3px solid #667eea;cursor:pointer;transition:all 0.2s}
.news-item:hover{background:#f0f4ff;transform:translateX(3px)}
.news-item .source{font-weight:bold;color:#667eea;font-size:12px}
.news-item .title{color:#333;font-size:14px;margin-top:3px}
</style>
</head>
<body>
<div class="container">
<div class="header">
<h1>🗞️ SEWU NEWS</h1>
<p>Konfigurasi Sumber Berita</p>
</div>
<div class="content">
<div class="info-box">
<strong>IP Address</strong>
<span id="ip">Loading...</span>
</div>

<div class="section">
<div class="section-title">📰 Pilih Media Berita</div>
<select id="media">
<option value="">-- Pilih Media --</option>
<option value="media:detik.com">Detik.com</option>
<option value="media:kompas.com">Kompas.com</option>
<option value="media:kompas.id">Kompas.id</option>
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

<div class="section">
<div class="section-title">🏷️ Pilih Kategori</div>
<select id="kategori">
<option value="">-- Pilih Kategori --</option>
<option value="headline">📌 Top Headlines</option>
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

<div class="section">
<div class="section-title">🔍 Custom Search</div>
<input type="text" id="customSearch" placeholder="Ketik keyword: jokowi, prabowo, gempa, dll...">
</div>

<button class="btn" onclick="save()">💾 Simpan & Update Berita</button>
<div class="status" id="status"></div>

<div class="section" style="margin-top:30px">
<div class="section-title">📋 Preview Berita (Live di P10)</div>
<div class="news-list" id="newsList">Loading...</div>
</div>
</div>
</div>
</div>

<script>
function $(id){return document.getElementById(id)}

function showStatus(msg,ok){
  var s=$('status');
  s.textContent=msg;
  s.className='status '+(ok?'success':'error');
  setTimeout(()=>{s.style.display='none'},3000);
}

function loadNews(){
  fetch('/api/news')
  .then(r=>r.json())
  .then(d=>{
    var list=$('newsList');
    if(d.count==0){
      list.innerHTML='<div style="text-align:center;color:#999">Belum ada berita</div>';
      return;
    }
    var html='';
    d.items.forEach((n,i)=>{
      var hasLink = n.link && n.link.length > 5;
      var clickAttr = hasLink ? `style="cursor:pointer" onclick='window.open("${n.link.replace(/"/g,'&quot;')}","_blank")'` : 'style="cursor:default;opacity:0.6"';
      var linkIcon = hasLink ? '🔗 ' : '';
      html+=`<div class="news-item" ${clickAttr}>
        <div class="source">${linkIcon}${i+1}. ${n.source}</div>
        <div class="title">${n.title}</div>
      </div>`;
    });
    list.innerHTML=html;
  })
  .catch(e=>{
    console.error(e);
    $('newsList').innerHTML='<div style="text-align:center;color:#f00">Error loading news</div>';
  });
}

function load(){
  fetch('/api/data')
  .then(r=>r.json())
  .then(d=>{
    $('ip').textContent=d.ip;
    var src=d.newsSource;
    
    // Set dropdown value
    if(src.startsWith('media:')){
      $('media').value=src;
      $('kategori').value='';
      $('customSearch').value='';
    }else if(src.startsWith('search:')){
      var keyword=src.substring(7);
      // Cek apakah ada di dropdown kategori
      if($('kategori').querySelector(`option[value="search:${keyword}"]`)){
        $('kategori').value=src;
      }else{
        // Custom search
        $('customSearch').value=keyword;
      }
      $('media').value='';
    }else{
      $('kategori').value=src;
      $('media').value='';
      $('customSearch').value='';
    }
  });
  
  loadNews();
}

function save(){
  var media=$('media').value;
  var kat=$('kategori').value;
  var custom=$('customSearch').value.trim();
  
  var source='headline';
  
  if(custom){
    // Custom search prioritas tertinggi
    source='search:'+custom;
  }else if(media){
    source=media;
  }else if(kat){
    source=kat;
  }
  
  fetch('/api/config',{
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body:JSON.stringify({newsSource:source})
  })
  .then(r=>r.json())
  .then(d=>{
    if(d.ok){
      showStatus('✅ Tersimpan! Berita akan diupdate dalam 10 detik...',true);
      setTimeout(loadNews,11000); // Reload news setelah 11 detik
    }else{
      showStatus('❌ Gagal simpan',false);
    }
  });
}

// Auto-clear saat pilih dropdown
$('media').onchange=function(){if(this.value){$('kategori').value='';$('customSearch').value='';}};
$('kategori').onchange=function(){if(this.value){$('media').value='';$('customSearch').value='';}};
$('customSearch').oninput=function(){if(this.value){$('media').value='';$('kategori').value='';}};

// Auto refresh news preview tiap 30 detik
setInterval(loadNews,30000);

window.onload=load;
</script>
</body>
</html>
)=====";
