const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
const mqtt = require('mqtt');

// Connect to MongoDB database
mongoose.connect('mongodb://localhost/door-monitor', {
  useNewUrlParser: true,
  useUnifiedTopology: true,
});

// Define schema for count data
const countSchema = new mongoose.Schema({
  count: {
    type: Number,
    required: true,
  },
  timestamp: {
    type: Date,
    default: Date.now,
  },
});

// Define model for count data
const Count = mongoose.model('Count', countSchema);

// Connect to MQTT broker
const mqttClient = mqtt.connect('mqtt://localhost:1883');
mqttClient.on('connect', function () {
  mqttClient.subscribe('door-count');
});

// Handle incoming MQTT messages
mqttClient.on('message', function (topic, message) {
  // Convert message to integer
  const count = parseInt(message.toString(), 10);

  // Save count data to database
  const newCount = new Count({ count });
  newCount.save(function (err) {
    if (err) {
      console.error('Failed to save count data:', err);
    } else {
      console.log('Saved count data:', newCount);
    }
  });
});

// Create Express app
const app = express();
app.use(bodyParser.json());

// Define routes
app.get('/counts', async function (req, res) {
  try {
    // Get latest 10 count data from database
    const counts = await Count.find({})
      .sort({ timestamp: -1 })
      .limit(10)
      .lean();

    // Send count data as JSON response
    res.json(counts);
  } catch (err) {
    console.error('Failed to get count data:', err);
    res.status(500).send('Failed to get count data');
  }
});

// Start server
const port = 3000;
app.listen(port, function () {
  console.log(`Server listening on port ${port}`);
});
