const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;
const MY_API_KEY = "0913"; // 設定一個密碼

app.use(cors());
app.use(bodyParser.json());

// 前端地圖頁面
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'map.html'));
});

app.get('/map.html', (req, res) => {
  res.sendFile(path.join(__dirname, 'map.html'));
});

app.post('/submit', (req, res) => {
  // 驗證 API Key
  if (req.headers['x-api-key'] !== MY_API_KEY) {
    return res.status(403).send('Forbidden: Invalid API Key');
  }

  const { timestamp, preference } = req.body;

  // 這裡就是您接收到的資料，您可以把它存入資料庫或寫入檔案
  console.log(`收到資料 - 時間戳: ${timestamp}, 偏好: ${preference}`);

  res.status(200).send('Data received successfully');
});

app.listen(PORT, () => {
  console.log(`伺服器運行中: http://localhost:${PORT}`);
  console.log(`地圖頁面: http://localhost:${PORT}/`);
});
