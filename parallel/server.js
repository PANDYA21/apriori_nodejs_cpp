const express = require('express');
let app = express();
const { getRecommendations } = require('.');

app.get('/api', (req, res, next) => {
  if (!req.query.product_id) {
    return res.status(400).json({ error: 'product_id not supplied with query' });
  }
  const ans = getRecommendations();
  res.status(200).send(ans);
});

app.listen(8081);