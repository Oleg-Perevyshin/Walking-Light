import {h, Component} from "preact";
import ManagerMain from "../pages/Manager";
import ManagerLeft from "../pages/ManagerLP";
import ManagerRight from "../pages/ManagerRP";
import NetworkMain from "../pages/Network";
import NetworkLeft from "../pages/NetworkLP";
import NetworkRight from "../pages/NetworkRP";
import ControlPage from "../pages/Control";
import ControlLP from "../pages/ControlLP";
import ControlRP from "../pages/ControlRP";
import ScriptsPage from "../pages/Scripts";
import ScriptsLP from "../pages/ScriptsLP";
import ScriptsRP from "../pages/ScriptsRP";
import SettingsMain from "../pages/Settings";
import SettingsLeft from "../pages/SettingsLP";
import SettingsRight from "../pages/SettingsRP";
import {fetchJSON, fetchText} from "../util";
import {stringify} from "querystring";
import Message from "./Message";
import Body from "./Body";
import autobind from 'autobind-decorator';

@autobind
class App extends Component {
  constructor() {
    super();
    this.setState({
      actualInfo: {},
      wifiScan: [{}],
      dl: null,
      dls: [],
      dv: null,
      networks: [],
      dictionary: {},
      device: {},
      devices: {},
      colorSorting: null,
      typeSorting: null,
      loading: true,
      ws: new WebSocket('ws://' + location.hostname + '/ws', ['soi-tech']),
      attempts: 1
    });
  }

  getChildContext() {
    return {
      root: this,

      setParam: ::this.setParam,
      linkState: ::this.linkState,

      colorSorting: this.state.colorSorting,
      typeSorting: this.state.typeSorting,
      setColorSorting: ::this.setColorSorting,
      setTypeSorting: ::this.setTypeSorting,
      clearSorting: ::this.clearSorting,

      dictionary: this.state.dictionary,
      device: this.state.device,
      devices: this.state.devices,

      networks: this.state.networks,
      actualInfo: this.state.actualInfo,
      wifiScan: this.state.wifiScan,
      setLang: ::this.setLang,
      setTheme: ::this.setTheme,
      dls: this.state.dls,
      dl: this.state.dl,
      dv: this.state.dv,

      sendWS: ::this.sendWS,
      resWS: ::this.resWS
    }
  }

  // Изменение состояния устройства
  setParam(param, value) {
    const {device} = this.state;
    device[param] = value;
    this.setState({device});
  }

  // Сортировка по цвету
  setColorSorting(value) {
    this.setState({colorSorting: value, typeSorting: null});
  }

  // Сортировка по типу
  setTypeSorting(value) {
    this.setState({colorSorting: null, typeSorting: value});
  }

  // Сброс параметров сортировки
  clearSorting() {
    this.setState({colorSorting: null, typeSorting: null});
  }

  componentDidMount() {
    this.setupWebsocket();
  }

  componentWillUnmount() {
    this.shouldReconnect = false;
    clearTimeout(this.timeoutID);
    let websocket = this.state.ws;
    websocket.close();
  }

  setupWebsocket() {
    let websocket = this.state.ws;

    websocket.onopen = () => {
    };

    websocket.onmessage = (evt) => {
      this.resWS(JSON.parse(evt.data));
    };

    this.shouldReconnect = this.props.reconnect;
    websocket.onclose = () => {
      if (typeof this.props.onClose === 'function') this.props.onClose();
      if (this.shouldReconnect) {
        this.timeoutID = setTimeout(() => {
          this.setState({attempts: this.state.attempts + 1});
          this.setState({ws: new WebSocket('ws://' + location.hostname + '/ws', ['soi-tech'])});
          this.setupWebsocket();
        }, 100);
      }
    }
  }

  // ОБРАБОТКА ВХОДЯЩИХ ПАКЕТОВ WEBSOCKET testForWS
  resWS(data) {
    if (data.actualInfo) {                                          // АКТУАЛЬНЫЕ ПАРАМЕТРЫ УСТРОЙСТВА
      this.setState({actualInfo: JSON.parse(data.actualInfo)});
    } else if (data.wifiScan) {                                     // ПОЛУЧИЛИ СПИСОК ДОСТУПНЫХ ТОЧЕК ДОСТУПА
      this.setState({networks: JSON.parse(data.wifiScan)});
    } else if (data.devsList) {                                     // ПОЛУЧИЛИ СПИСОК УСТРОЙСТВ В СЕТИ
      this.setState({devices: JSON.parse(data.devsList)});
    }
  }

  // ФОРМИРОВАНИЕ ПАКЕТОВ ДЛЯ ОТПРАВКИ В WEBSOCKET
  sendWS(event) {
    let data;
    if (event === "actualInfo") {                                   // СВОДНЫЕ ДАННЫЕ ОБ УСТРОЙСТВЕ
      data = `{"actualInfo":[]}`;
    } else if (event === "wifiScan") {                              // СКАНИРОВАНИЕ ТОЧЕК ДОСТУПА
      data = `{"wifiScan":[]}`;
    } else if (event === "devsScan") {                              // СКАНИРОВАНИЕ УСТРОЙСТВ В СЕТИ
      data = `{"devsScan":[]}`;
    } else if (event === "setID") {                                 // УСТАНОВКА ИДЕНТИФИКАЦИОННЫХ ПАРАМЕТРОВ УСТРОЙСТВА
      const {dc, dn, du, dp} = this.state.device;
      data = `{"setID":["${dc}","${dn}","${du}","${dp}"]}`;
    } else if (event === "setSTA") {                                // УСТАНОВКА ПАРАМЕТРОВ РЕЖИМ "КЛИЕНТ"
      const {staSSID, staPSK, staIPMode, staIP, staMS, staGW} = this.state.device;
      data = `{"setSTA":["${staSSID}","${staPSK}","${staIPMode}","${staIP}","${staMS}","${staGW}"]}`;
    } else if (event === "setAP") {                                 // УСТАНОВКА ПАРАМЕТРОВ РЕЖИМ "ТОЧКА ДОСТУПА"
      const {apSSID, apPSK, apIP, apMS, apGW} = this.state.device;
      data = `{"setAP":["${apSSID}","${apPSK}","${apIP}","${apMS}","${apGW}"]}`;
    } else if (event === "setRF") {                                 // УСТАНОВКА ПАРАМЕТРОВ ПАРАМЕТРЫ РАДИО СЕТИ
      const {da, rch} = this.state.device;
      data = `{"setRF":["${da}","${rch}"]}`;
    } else if (event === "dl") {                                    // УСТАНОВКА ЯЗЫКА ИНТЕРФЕЙСА (не работает в Web)
      const {dl} = this.state.dl;
      data = `{"dl":["${dl}"]}`;
    } else if (event === "dv") {                                    // УСТАНОВКА ВИЗУАЛЬНОЙ ТЕМЫ (не работает в Web)
      const {dv} = this.state.dv;
      data = `{"dv":["${dv}"]}`;
    } else if (event === "synch") {                                // ПЕРЕДАЧА ПАРАМЕТРОВ АВТОРИЗАЦИИ (для рассылки по радио сети)
      data = `{"synch":[]}`;
    } else if (event === "rst") {                                   // ПЕРЕЗАГРУЗКА УСТРОЙСТВА
      data = `{"rst":[]}`;
    } else if (event === "def") {                                   // СБРОС НАСТРОЕК ПО УМОЛЧАНИЮ
      data = `{"def":[]}`;
    } else {
      let id = event.target.id;
      let val = event.target.value;
      data = `{"mcu":["${id}","${val}"]}`;
    }
    let websocket = this.state.ws;
    websocket.send(data);
  }

  // ИНТЕРФЕЙС (язык)
  setLang(event) {
    return fetchJSON(`/lang?dl=${event.target.value}.json`)
      .then(dictionary => {
        this.setState({dictionary, dl: event.target.value});
        fetch(`/interface?dl=${event.target.value}`)
          .catch(e => {
            console.error(e);
          });
      })
      .catch(e => {
        console.error(e);
      });
  }

  // ИНТЕРФЕЙС (тема)
  setTheme(event) {
    let dv = event.target.value;
    this.setState({dv: dv});
    this.sendWS("dv");
    return fetch(`/interface?dv=${dv}`)
      .catch(e => {
        console.error(e);
      });
  }

  componentWillMount() {
    window.addEventListener('hashchange', ::this.onHashChange);
    this.onHashChange();
    Promise.all([
      fetchJSON('/config.json')
        .then(device => {
          const langList = JSON.parse(device.dls);
          this.setState({device});
          this.setState({dv: device.dv});
          this.setState({dls: langList.sort()});
          return fetchJSON(`/lang?dl=${device.dl}.json`)
            .then(dictionary => this.setState({dictionary, dl: device.dl}))
            .catch(e => {
              console.error(e);
            });
        })
        .catch(e => {
          console.error(e);
        })
    ])
      .then(() => this.setState({loading: false}));
  }

  onHashChange() {
    let {hash} = window.location;
    switch (hash) {
      case '#network':
      case '#control':
      case '#scripts':
      case '#settings':
        break;
      default:
        hash = '';
        break;
    }
    this.setState({activeRoute: hash});
  }

  render(props, {activeRoute, device, testForWS}) {
    return (
      <div id="wrapper">
        <Body dv={this.state.dv || null} loading={this.state.loading}/>
        {!this.state.loading &&
        <div>
          <a target="_blank" href="http://soi-tech.com">
            <div className="info" hidden>
              <p className="bold"><Message key="messages.low_resolution"/></p>
            </div>
            <div className="logo"/>
          </a>
          <a target="_blank" href="http://www.soi-tech.com/walkinglight/">
            <div className="dev"/>
          </a>
          <div className="main">
            <input id="lbch" type="checkbox" checked/>
            <label id="lCheck" for="lbch" className="lbcheck"/>
            <div className="lbar">
              {activeRoute === '' && <ManagerLeft/>}
              {activeRoute === '#network' && <NetworkLeft/>}
              {activeRoute === '#control' && <ControlLP/>}
              {activeRoute === '#scripts' && <ScriptsLP/>}
              {activeRoute === '#settings' && <SettingsLeft/>}
            </div>
            <input id="rbch" type="checkbox" checked/>
            <label id="rCheck" for="rbch" className="rbcheck"/>
            <div className="rbar">
              {activeRoute === '' && <ManagerRight/>}
              {activeRoute === '#network' && <NetworkRight/>}
              {activeRoute === '#control' && <ControlRP/>}
              {activeRoute === '#scripts' && <ScriptsRP/>}
              {activeRoute === '#settings' && <SettingsRight/>}
            </div>
            <h1 id="nameDF" className="header">{device.dm} | {device.dn}</h1>
            <div id="menu">
              <ul>
                <li>
                  <a href="#" className={activeRoute === '' ? 'active' : ''}>
                    <div class="icon-container">
                      <div className="icon icon-manager"/>
                    </div>
                    <span className="mt"><Message key="menu.manager"/></span>
                  </a>
                </li>
                <li>
                  <a href="#network" className={activeRoute === '#network' ? 'active' : ''}>
                    <div class="icon-container">
                      <div className="icon icon-network"/>
                    </div>
                    <span className="mt"><Message key="menu.network"/></span>
                  </a>
                </li>
                <li>
                  <a href="#control" className={activeRoute === '#control' ? 'active' : ''}>
                    <div class="icon-container">
                      <div className="icon icon-control"/>
                    </div>
                    <span className="mt"><Message key="menu.control"/></span>
                  </a>
                </li>
                <li>
                  <a href="#scripts" className={activeRoute === '#scripts' ? 'active' : ''}>
                    <div class="icon-container">
                      <div className="icon icon-script"/>
                    </div>
                    <span className="mt"><Message key="menu.scripts"/></span>
                  </a>
                </li>
                <li>
                  <a href="#settings" className={activeRoute === '#settings' ? 'active' : ''}>
                    <div class="icon-container">
                      <div className="icon icon-settings"/>
                    </div>
                    <span className="mt"><Message key="menu.settings"/></span>
                  </a>
                </li>
              </ul>
            </div>
            <div className="cdl" style={`background-color: #${device.dc}`}/>
            <div id="content" className="content">
              {activeRoute === '' && <ManagerMain/>}
              {activeRoute === '#network' && <NetworkMain/>}
              {activeRoute === '#control' && <ControlPage/>}
              {activeRoute === '#scripts' && <ScriptsPage/>}
              {activeRoute === '#settings' && <SettingsMain/>}
            </div>
            <div className="footer">
              <Message key="footer.prefix"/>
              <a target="_blank" href="http://soi-tech.com">SOI Tech</a>
              <Message key="footer.suffix"/>
            </div>
          </div>
        </div>
        }
      </div>
    );
  }
}

export default App;
