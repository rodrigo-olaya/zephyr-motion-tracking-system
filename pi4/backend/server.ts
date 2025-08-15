import express from 'express';
import cors from 'cors';
import fs from 'fs'
import path from 'path';
import { SerialPort, ReadlineParser } from 'serialport'

import dotenv from 'dotenv';
dotenv.config();

const app = express();

const allowedOrigins = process.env.NODE_API_URL ? process.env.NODE_API_URL.split(',') : [];

console.log('Allowed Origins:', allowedOrigins);

const corsOptions = {
    origin: allowedOrigins,
};

// const corsOptions = {
//     origin: ['http://192.168.12.109:5173',
//       'http://localhost:5173']
// }

app.use(cors(corsOptions));
app.use(express.json());

// POST
import { Request, Response } from 'express';

// GET
const values: string[] = [];

const serialport = new SerialPort({ path: '/dev/serial0', baudRate: 2400 })

const parser = serialport.pipe(new ReadlineParser())

parser.on('data', (data: string) => {
  values.push(data);
})

app.get('/api/data', (req: Request, res: Response): void => {
  res.json(values);
})

app.listen(8080, () => {
  console.log('Server is running on 8080');
});