import React, { useState, useEffect } from 'react';
import ChannelsProvider from 'phoenix-react'
import './App.scss';
import './styles/reduction.scss';

import { Row, Col, Card, CardHeader, CardBody } from 'reactstrap';
import { Line, Pie } from 'react-chartjs-2';
import { getColor } from './utils/colors';
import { IconWidget } from './components/Widget';
import Switch from "react-switch";
import { Range } from 'react-range';



import {
  MdLightbulbOutline,
  MdVibration,
  MdPanTool
} from 'react-icons/md';


let temp_row = [];
const max_values_num = 50;

const genLineData = (value, ref) => {
  if (temp_row.length < max_values_num) {
    for (let i = temp_row.length; i < max_values_num; i++) {
      temp_row.push(0);
    }
  }
  if (value && value > 0) {
    if (temp_row.length >= max_values_num) {
      temp_row.shift(value);
    }
    temp_row.push(value);
  }
  if (ref && ref.chartInstance) {
    ref.chartInstance.update();
  }
  let labels = [];
  for (let i = 0; i < temp_row.length; i++) {
    labels.push('');
  }
  return {
    labels: labels,
    datasets: [
      {
        label: 'Текущая температура',
        backgroundColor: getColor('success'),
        borderColor: getColor('black'),
        borderWidth: 1,
        type: 'line',
        fill: false,
        data: temp_row,
      }
    ],
  };
};

const genPieData = (val) => {
  return {
    datasets: [
      {
        data: [val, 100 - val],
        backgroundColor: [
          getColor('primary'),
          getColor('secondary')
        ],
        label: 'Процентов',
      },
    ],
    labels: ['До GND', 'До +5V'],
  };
};


function App(props) {
  let chart_ref = null;

  return (
    <div className="App">
      <Row>
        <Col xl={6} lg={12} md={12}>
          <Card>
            <CardHeader>Температура</CardHeader>
            <CardBody>

              <ChannelsProvider.Consumer>
                {({ state }) => (
                  <Line
                    data={genLineData(state.temperature, chart_ref)}
                    events={[]}
                    ref={(reference) => chart_ref = reference}
                    options={{
                      scales: {
                        xAxes: [
                          {
                            scaleLabel: {
                              display: true,
                              labelString: 'Время',
                            },
                          },
                        ],
                        yAxes: [
                          {
                            stacked: true,
                            scaleLabel: {
                              display: true,
                              labelString: 'Значение',
                            },
                          },
                        ],
                      },
                    }}
                  />)}
              </ChannelsProvider.Consumer>
            </CardBody>
          </Card>
        </Col>

        <Col xl={6} lg={12} md={12}>
          <Card>
            <CardHeader>Потенциометр</CardHeader>
            <CardBody>
              <ChannelsProvider.Consumer>
                {({ state }) => (<Pie data={genPieData(state.potent)} />)}
              </ChannelsProvider.Consumer>
            </CardBody>
          </Card>
        </Col>
      </Row>

      <br />
      <Row>
        <Col lg={4} md={6} sm={6} xs={12} className="mb-3">
          <ChannelsProvider.Consumer>
            {({ state }) => (state.is_light ?
              <IconWidget
                bgColor="light"
                icon={MdLightbulbOutline}
                title="Свет включен"
                subtitle="Датчик освещенности"
                inverse={false}
              />
              :
              <IconWidget
                bgColor="dark"
                icon={MdLightbulbOutline}
                title="Свет выключен"
                subtitle="Датчик освещенности"
                inverse={true}
              />
            )}
          </ChannelsProvider.Consumer>
        </Col>
        <Col lg={4} md={6} sm={6} xs={12} className="mb-3">
          <ChannelsProvider.Consumer>
            {({ state }) => (state.is_motion ?
              <IconWidget
                bgColor="danger"
                icon={MdVibration}
                title="Движение есть"
                subtitle="Датчик движения"
              />
              :
              <IconWidget
                bgColor="dark"
                icon={MdVibration}
                title="Движения нет"
                subtitle="Датчик движения"
              />
            )}
          </ChannelsProvider.Consumer>
        </Col>
        <Col lg={4} md={6} sm={6} xs={12} className="mb-3">
          <ChannelsProvider.Consumer>
            {({ state }) => (state.is_barrier ?
              <IconWidget
                bgColor="warning"
                icon={MdPanTool}
                title="Препятствие есть"
                subtitle="Датчик препятствия"
              />
              :
              <IconWidget
                bgColor="dark"
                icon={MdPanTool}
                title="Препятствия нет"
                subtitle="Датчик препятствия"
              />
            )}
          </ChannelsProvider.Consumer>

        </Col>
      </Row>

      <br />
      <Row>
        <Col lg={4} md={6} sm={6} xs={12} className="mb-3">
          <div className="card">
            <div className="card-body">
              <ChannelsProvider.Consumer>
                {({ state }) => (
                  <>Датчик расстояния: {state.range}см<br /><br />
                    <Range
                      step={1}
                      min={0}
                      max={150}
                      values={[Math.min(state.range, 150)]}
                      onChange={values => { }}
                      renderTrack={({ props, children }) => (
                        <div
                          {...props}
                          style={{
                            ...props.style,
                            height: '3px',
                            width: '100%',
                            backgroundColor: '#ccc'
                          }}
                        >
                          {children}
                        </div>
                      )}
                      renderThumb={({ props }) => (
                        <div
                          {...props}
                          style={{
                            ...props.style,
                            height: '12px',
                            width: '12px',
                            backgroundColor: '#999'
                          }}
                        />
                      )}
                    />
                  </>)}
              </ChannelsProvider.Consumer>

            </div>
          </div>

        </Col></Row>


      <Row>
        <Col lg={4} md={6} sm={6} xs={12} className="mb-3">
          <div className="card">
            <div className="card-body">
              Светодиод:<br /><br />
              <ChannelsProvider.Consumer>
                {({ state, mutate, fire }) => (
                  <Switch
                    className="react-switch"
                    checked={!!state.simple_led}
                    onChange={(val) => {
                      fire("iotmeetup:data", "command", { command: "simple_led", value: val });
                      mutate(state => { state.simple_led = val; return state; });
                    }}
                  />
                )}
              </ChannelsProvider.Consumer>
            </div>
          </div>
        </Col>
        <Col lg={4} md={6} sm={6} xs={12} className="mb-3">
          <div className="card">
            <div className="card-body">
              220 Вольт:<br /><br />
              <ChannelsProvider.Consumer>
                {({ state, mutate, fire }) => (
                  <Switch
                    className="react-switch"
                    checked={!!state.sock_220v}
                    onChange={(val) => {
                      fire("iotmeetup:data", "command", { command: "sock_220v", value: val });
                      mutate(state => { state.sock_220v = val; return state; });
                    }}
                  />
                )}
              </ChannelsProvider.Consumer>
            </div>
          </div>
        </Col>
        <Col lg={4} md={6} sm={6} xs={12} className="mb-3">
          <div className="card">
            <div className="card-body">
              Сервопривод <br /><br />
              <ChannelsProvider.Consumer>
                {({ state, mutate, fire }) => (
                  <Range
                    step={1}
                    min={0}
                    max={100}
                    values={[state.servo]}
                    onChange={(values) => {
                      fire("iotmeetup:data", "command", { command: "servo", value: values[0] });
                      mutate(state => { state.servo = values[0]; return state; });
                    }}
                    renderTrack={({ props, children }) => (
                      <div
                        {...props}
                        style={{
                          ...props.style,
                          height: '6px',
                          width: '100%',
                          backgroundColor: '#ccc'
                        }}
                      >
                        {children}
                      </div>
                    )}
                    renderThumb={({ props }) => (
                      <div
                        {...props}
                        style={{
                          ...props.style,
                          height: '25px',
                          width: '25px',
                          backgroundColor: '#999'
                        }}
                      />
                    )}
                  />
                )}
              </ChannelsProvider.Consumer>
            </div>
          </div>
        </Col>
      </Row>

    </div >
  );
}

export default App;
