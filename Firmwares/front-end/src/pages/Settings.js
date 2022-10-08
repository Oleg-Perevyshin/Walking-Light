// Страница "ОБНОВЛЕНИЕ" (центр)

import {h, Component} from "preact";
import Interface from "../components/Interface";
import IDConfig from "../components/IDConfig";
import Service from "../components/Service";
import Input from "../components/Input";
import Message from "../components/Message";
import Button from "../components/Button";
import {fetchJSON, fetchText} from "../util";
import {stringify} from "querystring";


export default class SettingsMain extends Component {
    componentWillMount() {
        this.setState({
            service: {},
            expanded: {
                update: false
            },
            updating: false,
            updateAvailable: false
        });
        this.checkUpdates();
    }

    checkUpdates() {
        return fetchJSON("/ota/check")
            .then(available => {
                this.setState({updateAvailable: available});
            })
            .catch(e => {
                console.error(e);
            });
    }

    otaUpdate() {
        fetchText("/ota/upd")
            .then(this.setState({updating: true}))
            .catch(e => {
                console.error(e);
            });
    }

// **********************************************************
// Для теста
    onFileUpload(event) {
        this.setState({file: event.target.files[0]});
        const {file} = this.state;
        const data = new FormData;
        data.append("data", file);
        fetch("/mcu/upload", {
            method: "POST",
            body: data
        })
            .catch(e => {
                console.log(e);
            });
    }

    mcuUpdate() {
        fetchText("/mcu/upd")
            .catch(e => {
                console.error(e);
            });
    }

    render({}, {file}) {

        return (
            <div>
                <h2 className="tPan"><Message key="s.m.t"/></h2>
                <div>

                    <Interface/>
                    <IDConfig/>

                    {/* ОБНОВЛЕНИЕ */}
                    <div className="sb">
                        <input type="checkbox" id="update" className="checker"
                               checked={this.state.expanded.update}
                               onChange={this.linkState("expanded.update")}/>
                        <label for="update"><p className="tsb"><Message key="s.m.update.t"/></p></label>
                        <div className="spl">
                            <br/>
                            {this.state.updateAvailable ?
                                <div>
                                    <p className="bold"><Message key="s.m.update.available"/></p>
                                    <Button className="button w75 green large" onClick={::this.otaUpdate}
                                            disabled={!this.state.updateAvailable}>
                                        <Message key="a.upd"/>
                                    </Button>
                                </div>
                                :
                                <div>
                                    <p className="bold"><Message key="s.m.update.noavailable"/></p>
                                </div>
                            }
                        </div>

                        {/* ОБНОВЛЕНИЕ (ТЕСТ) */}
                        <div className="spl">
                            <p className="bold">Обновление MCU (отдельно)</p>
                            <label className="fileform w75">
                                <span id="ul">{file ? file["name"] : "Выберите файл"}</span>
                                <Input className="input large" type="file" name="upload" id="upload"
                                       onChange={::this.onFileUpload}/>
                            </label>
                            <Button className="button w75 green large" onClick={::this.mcuUpdate}>Обновить</Button>
                            <br/>
                        </div>
                    </div>

                    <Service/>

                    {/* Перезагрузка */}
                    <Button className="button w75 red large" onClick={() => this.context.sendWS("rst")}><Message
                        key="a.rst"/></Button>

                </div>
            </div>
        );
    }
}
